#include "AstDumper.hpp"

#include <Luau/Ast.h>
#include <Luau/AstJsonEncoder.h>
#include <Luau/Parser.h>

bool AstDumper::dump() {
    Luau::Allocator allocator;
    Luau::AstNameTable names(allocator);
    Luau::ParseResult result =
        Luau::Parser::parse(inputString.c_str(), inputString.size(), names,
                            allocator, Luau::ParseOptions());

    if (!result.errors.empty()) {
        // Users of this function expect only a single error message
        const Luau::ParseError& parseError = result.errors.front();
        std::string error =
            Luau::format(":%d: %s", parseError.getLocation().begin.line + 1,
                         parseError.what());

        this->astJson = error;
        return false;
    }

    this->astJson = Luau::toJson(result.root, result.commentLocations);
    return true;
}

const std::string& AstDumper::getAstJson() { return this->astJson; }
