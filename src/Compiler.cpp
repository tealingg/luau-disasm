#include "Compiler.hpp"

#include <Luau/BytecodeBuilder.h>
#include <Luau/Compiler.h>

static int encodeMult = 1;
class _ : public Luau::BytecodeEncoder {
public:
  void encode(std::uint32_t *data, size_t count) override {
    for (size_t i = 0; i < count; i++) {
      // multiply just the first byte of the instruction by the encode
      // multiplier, and leave the rest of the instruction unchanged
      std::uint32_t opcode = LUAU_INSN_OP(data[i]);
      std::uint32_t newOpcode = (opcode * encodeMult) % 256;
      data[i] = (data[i] & 0xffffff00) | newOpcode; // hopefully.
    }
  }
} encoder;

bool Compiler::compile() {
  encodeMult = 1;
  if (this->shouldEncode) {
    encodeMult = this->encodeMultiplier;
  }

  this->bytecode = Luau::compile(this->inputString, {}, {}, &encoder);

  if (this->bytecode.at(0) == '\0') { // syntax error
    return false;
  }

  return true;
}

const std::string &Compiler::getBytecode() { return this->bytecode; }