#include <filesystem>
#include <fstream>
#include <sstream>

#include "ArgParser.hpp"
#include "AstDumper.hpp"
#include "Compiler.hpp"
#include "Disassembler.hpp"

namespace fs = std::filesystem;

enum DisasmMode { disassemble, compile, dump };

void printUsage(const std::string& fileName) {
    std::printf("Usage: %s [--options] -f <input file>\n", fileName.c_str());
    std::printf("\n");
    std::printf("Available options:\n");
    std::printf("\t-h, --help: Display this help message.\n");
    std::printf(
        "\t-c, --compile: Enable compilation mode. Requires -o <output "
        "file>.\n");
    std::printf(
        "\t-d, --dump-ast: Dump the AST of the input source file as JSON.\n");
    std::printf(
        "\t-f <input file>, --file <input file>: Provide a Luau "
        "bytecode/source file to be disassembled/compiled.\n");
    std::printf(
        "\t-o <output file>, --output <output file>: Output file for "
        "disassembly/compilation (NOTE: required for compilation mode).\n");
    std::printf(
        "\t-e <multiplier>, --encode <multiplier>: Multiplier for "
        "encoding/decoding instructions.\n");
}

static int assertionHandler(const char* expr, const char* file, int line,
                            const char* function) {
    printf("%s(%d): ASSERTION FAILED: %s\n", file, line, expr);
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::printf("Error: no input file specified.\n");
        printUsage(argv[0]);
        return 0;
    }

    ArgParser parser(argc, argv);
    DisasmMode mode = DisasmMode::disassemble;
    std::string fileName;
    bool shouldOutput = false;
    std::string outputFileName = "";
    fs::path inputFilePath;
    std::ifstream inputFileData;
    std::stringstream inputFileStream;
    std::ofstream outputFileStream;
    int encodeMult = 1;

    if (parser.doesArgExist("-h") || parser.doesArgExist("--help")) {
        printUsage(argv[0]);
        return 0;
    }

    if (!parser.doesArgExist("-f") && !parser.doesArgExist("--file")) {
        std::printf("Error: no input file specified.\n");
        printUsage(argv[0]);
        return 0;
    }

    if (parser.doesArgExist("-c") || parser.doesArgExist("--compile")) {
        if (!parser.doesArgExist("-o") && !parser.doesArgExist("--output")) {
            std::printf(
                "Error: please specify output file using `-o <filename>`\n");
            return 0;
        }
        mode = DisasmMode::compile;
    } else if (parser.doesArgExist("-d") || parser.doesArgExist("--dump-ast")) {
        mode = DisasmMode::dump;
    }

    fileName = parser.getArgValue("-f");
    if (fileName.empty()) {
        fileName = parser.getArgValue("--file");
    }

    if (parser.doesArgExist("-o") || parser.doesArgExist("--output")) {
        shouldOutput = true;
        outputFileName = parser.getArgValue("-o");
        if (outputFileName.empty()) {
            outputFileName = parser.getArgValue("--output");
        }
    }

    if (parser.doesArgExist("-e") || parser.doesArgExist("--encode")) {
        std::string encodeMultStr = parser.getArgValue("-e");
        if (encodeMultStr.empty()) {
            encodeMultStr = parser.getArgValue("--encode");
        }
        encodeMult = std::stoi(encodeMultStr);
    }

    inputFilePath = fileName;

    if (!fs::exists(inputFilePath)) {
        std::printf("Error: input file %s not found.\n", fileName.c_str());
        return 0;
    }

    Luau::assertHandler() = assertionHandler;

    inputFileData =
        std::ifstream(inputFilePath, std::ios::in | std::ios::binary);
    inputFileStream << inputFileData.rdbuf();

    if (mode == DisasmMode::compile) {
        Compiler compiler(inputFileStream.str());

        if (!compiler.compile()) {
            std::printf("Syntax error: %s\n",
                        compiler.getBytecode().c_str() + 1);
            return 0;
        }

        outputFileStream.open(outputFileName, std::ios::out | std::ios::binary);
        outputFileStream << compiler.getBytecode();
        outputFileStream.close();

        std::printf("Bytecode written to %s!\n", outputFileName.c_str());
    } else if (mode == DisasmMode::dump) {
        AstDumper dumper(inputFileStream.str());

        if (!dumper.dump()) {
            std::printf("Error dumping AST: %s\n", dumper.getAstJson().c_str());
            return 0;
        }

        if (shouldOutput) {
            outputFileStream.open(outputFileName,
                                  std::ios::out | std::ios::binary);
            outputFileStream << dumper.getAstJson();
            outputFileStream.close();
        } else {
            std::printf("%s\n", dumper.getAstJson().c_str());
        }
    } else {
        Disassembler disassembler(inputFileStream.str(), encodeMult,
                                  encodeMult != 1);

        if (!disassembler.disassemble()) {
            std::printf("Disassembler error: input bytecode is invalid.\n");
            return 0;
        }

        if (shouldOutput) {
            outputFileStream.open(outputFileName,
                                  std::ios::out | std::ios::binary);
            outputFileStream << disassembler.getStreamData();
            outputFileStream.close();
        } else {
            std::printf("%s\n", disassembler.getStreamData().c_str());
        }
    }

    return 0;
}
