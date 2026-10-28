# luau-disasm
this is my really awful attempt at writing a luau disassembler.

luau-disasm can both compile lua(u) scripts into luau bytecode, and disassemble luau bytecode from files.

see [here](https://raw.githubusercontent.com/tealingg/luau-disasm/main/disassembled_example.txt) for the disassembly of a compiled version of Dex V2.

## disclaimers
- uh yeah expect bugs and stuff

## usage
luau-disasm currently does not provide prebuilt binaries and will require building from source. for now, this project isn't for the average end user.

```
Usage: luau-disasm [--options] -f <input file>

Available options:
        -h, --help: Display this help message.
        -c, --compile: Enable compilation mode. Requires -o <output file>.
        -f <input file>, --file <input file>: Provide a Luau bytecode/source file to be disassembled/compiled.
        -o <output file>, --output <output file>: Output file for disassembly/compilation (NOTE: required for compilation mode).
        -e <multiplier>, --encode <multiplier>: Multiplier for encoding/decoding instructions.
```

example (compilation):
```sh
luau-disasm -c -f input.lua -o output.luau -e 227 # roblox's opcode multiplier is 227
```

example (disassembly):
```sh
luau-disasm -f output.luau -e 227 # roblox bytecode
```

## building from source
- clone the repository and `cd` into it.
- create and enter a build directory
```
mkdir build
cd build
```
windows (using a vs developer command prompt):
```sh
cmake ..
cmake --build . --config Release # or RelWithDebInfo
```
macos/linux:
```sh
cmake .. -DCMAKE_BUILD_TYPE=Release # or RelWithDebInfo
cmake --build .
```
