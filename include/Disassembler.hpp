#include <sstream>
#include <string>
#include <vector>

#include <Luau/Bytecode.h>
#include <lobject.h>

class Disassembler {
private:
  std::string inputBytecode;
  std::stringstream stream;
  std::vector<std::uint8_t> opcodeDecodeTable;
  bool shouldDecode;

  void write(const char *format, ...);
  void decodeInstructions(Proto *p);
  void processProto(Proto *p, Proto *parent);

public:
  Disassembler(const std::string &input, int encodeMult = 1,
               bool decode = false)
      : inputBytecode(input), shouldDecode(decode) {
    this->opcodeDecodeTable = std::vector<std::uint8_t>(LOP__COUNT);
    for (int i = 0; i < LOP__COUNT; i++) {
      this->opcodeDecodeTable.push_back(std::uint8_t(i * encodeMult));
    }
  }
  bool disassemble();
  std::string getStreamData();
};