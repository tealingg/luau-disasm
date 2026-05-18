#include <string>

class Compiler {
private:
    std::string inputString;
    std::string bytecode;
    bool shouldEncode;
    int encodeMultiplier;

public:
    Compiler(const std::string& input, int encodeMult = 1, bool encode = false) : inputString(input), encodeMultiplier(encodeMult), shouldEncode(encode) {}
    bool compile();
    const std::string& getBytecode();
};