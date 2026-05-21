#include <Luau/Bytecode.h>
#include <lobject.h>

#include <cstdint>
#include <map>
#include <sstream>
#include <string>

class Disassembler {
   private:
    std::string inputBytecode;
    std::stringstream stream;
    std::map<std::uint8_t, std::uint8_t> opcodeDecodeTable;
    bool shouldDecode;

    void write(const char* format, ...);
    void decodeInstructions(Proto* p);
    void processProto(Proto* p, Proto* parent);

   public:
    Disassembler(const std::string& input, int encodeMult = 1,
                 bool decode = false)
        : inputBytecode(input), shouldDecode(decode) {
        this->opcodeDecodeTable = std::map<std::uint8_t, std::uint8_t>();
        for (int i = 0; i < LOP__COUNT; i++) {
            this->opcodeDecodeTable[std::uint8_t(i * encodeMult)] = i;
        }
    }
    bool disassemble();
    std::string getStreamData();
};
