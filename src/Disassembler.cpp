#include "Disassembler.hpp"

#include <lbytecode.h>
#include <lgc.h>
#include <lobject.h>
#include <lstate.h>
#include <lstring.h>
#include <lualib.h>
#include <string.h>

#include <algorithm>
#include <map>

const std::map<std::uint8_t, std::string> opcodeMap = {
    {LOP_NOP, "NOP"},
    {LOP_BREAK, "BREAK"},
    {LOP_LOADNIL, "LOADNIL"},
    {LOP_LOADB, "LOADB"},
    {LOP_LOADN, "LOADN"},
    {LOP_LOADK, "LOADK"},
    {LOP_MOVE, "MOVE"},
    {LOP_GETGLOBAL, "GETGLOBAL"},
    {LOP_SETGLOBAL, "SETGLOBAL"},
    {LOP_GETUPVAL, "GETUPVAL"},
    {LOP_SETUPVAL, "SETUPVAL"},
    {LOP_CLOSEUPVALS, "CLOSEUPVALS"},
    {LOP_GETIMPORT, "GETIMPORT"},
    {LOP_GETTABLE, "GETTABLE"},
    {LOP_SETTABLE, "SETTABLE"},
    {LOP_GETTABLEKS, "GETTABLEKS"},
    {LOP_SETTABLEKS, "SETTABLEKS"},
    {LOP_GETTABLEN, "GETTABLEN"},
    {LOP_SETTABLEN, "SETTABLEN"},
    {LOP_NEWCLOSURE, "NEWCLOSURE"},
    {LOP_NAMECALL, "NAMECALL"},
    {LOP_CALL, "CALL"},
    {LOP_RETURN, "RETURN"},
    {LOP_JUMP, "JUMP"},
    {LOP_JUMPBACK, "JUMPBACK"},
    {LOP_JUMPIF, "JUMPIF"},
    {LOP_JUMPIFNOT, "JUMPIFNOT"},
    {LOP_JUMPIFEQ, "JUMPIFEQ"},
    {LOP_JUMPIFLE, "JUMPIFLE"},
    {LOP_JUMPIFLT, "JUMPIFLT"},
    {LOP_JUMPIFNOTEQ, "JUMPIFNOTEQ"},
    {LOP_JUMPIFNOTLE, "JUMPIFNOTLE"},
    {LOP_JUMPIFNOTLT, "JUMPIFNOTLT"},
    {LOP_ADD, "ADD"},
    {LOP_SUB, "SUB"},
    {LOP_MUL, "MUL"},
    {LOP_DIV, "DIV"},
    {LOP_MOD, "MOD"},
    {LOP_POW, "POW"},
    {LOP_ADDK, "ADDK"},
    {LOP_SUBK, "SUBK"},
    {LOP_MULK, "MULK"},
    {LOP_DIVK, "DIVK"},
    {LOP_MODK, "MODK"},
    {LOP_POWK, "POWK"},
    {LOP_AND, "AND"},
    {LOP_OR, "OR"},
    {LOP_ANDK, "ANDK"},
    {LOP_ORK, "ORK"},
    {LOP_CONCAT, "CONCAT"},
    {LOP_NOT, "NOT"},
    {LOP_MINUS, "MINUS"},
    {LOP_LENGTH, "LENGTH"},
    {LOP_NEWTABLE, "NEWTABLE"},
    {LOP_DUPTABLE, "DUPTABLE"},
    {LOP_SETLIST, "SETLIST"},
    {LOP_FORNPREP, "FORNPREP"},
    {LOP_FORNLOOP, "FORNLOOP"},
    {LOP_FORGLOOP, "FORGLOOP"},
    {LOP_FORGPREP_INEXT, "FORGPREP_INEXT"},
    {LOP_FASTCALL3, "FASTCALL3"},
    {LOP_FORGPREP_NEXT, "FORGPREP_NEXT"},
    {LOP_NATIVECALL, "NATIVECALL"},
    {LOP_GETVARARGS, "GETVARARGS"},
    {LOP_DUPCLOSURE, "DUPCLOSURE"},
    {LOP_PREPVARARGS, "PREPVARARGS"},
    {LOP_LOADKX, "LOADKX"},
    {LOP_JUMPX, "JUMPX"},
    {LOP_FASTCALL, "FASTCALL"},
    {LOP_COVERAGE, "COVERAGE"},
    {LOP_CAPTURE, "CAPTURE"},
    {LOP_SUBRK, "SUBRK"},
    {LOP_DIVRK, "DIVRK"},
    {LOP_FASTCALL1, "FASTCALL1"},
    {LOP_FASTCALL2, "FASTCALL2"},
    {LOP_FASTCALL2K, "FASTCALL2K"},
    {LOP_FORGPREP, "FORGPREP"},
    {LOP_JUMPXEQKNIL, "JUMPXEQKNIL"},
    {LOP_JUMPXEQKB, "JUMPXEQKB"},
    {LOP_JUMPXEQKN, "JUMPXEQKN"},
    {LOP_JUMPXEQKS, "JUMPXEQKS"},
    {LOP_IDIV, "IDIV"},
    {LOP_IDIVK, "IDIVK"},
    {LOP_GETUDATAKS, "GETUDATAKS"},
    {LOP_SETUDATAKS, "SETUDATAKS"},
    {LOP_NAMECALLUDATA, "NAMECALLUDATA"},
    {LOP_NEWCLASSMEMBER, "NEWCLASSMEMBER"},
    {LOP_CALLFB, "CALLFB"},
};

std::map<std::uint8_t, std::string> builtinFunctionMap = {
    {LBF_NONE, "none"},
    {LBF_ASSERT, "assert"},
    {LBF_MATH_ABS, "math.abs"},
    {LBF_MATH_ACOS, "math.acos"},
    {LBF_MATH_ASIN, "math.asin"},
    {LBF_MATH_ATAN2, "math.atan2"},
    {LBF_MATH_ATAN, "math.atan"},
    {LBF_MATH_CEIL, "math.ceil"},
    {LBF_MATH_COSH, "math.cosh"},
    {LBF_MATH_COS, "math.cos"},
    {LBF_MATH_DEG, "math.deg"},
    {LBF_MATH_EXP, "math.exp"},
    {LBF_MATH_FLOOR, "math.floor"},
    {LBF_MATH_FMOD, "math.fmod"},
    {LBF_MATH_FREXP, "math.frexp"},
    {LBF_MATH_LDEXP, "math.ldexp"},
    {LBF_MATH_LOG10, "math.log10"},
    {LBF_MATH_LOG, "math.log"},
    {LBF_MATH_MAX, "math.max"},
    {LBF_MATH_MIN, "math.min"},
    {LBF_MATH_MODF, "math.modf"},
    {LBF_MATH_POW, "math.pow"},
    {LBF_MATH_RAD, "math.rad"},
    {LBF_MATH_SINH, "math.sinh"},
    {LBF_MATH_SIN, "math.sin"},
    {LBF_MATH_SQRT, "math.sqrt"},
    {LBF_MATH_TANH, "math.tanh"},
    {LBF_MATH_TAN, "math.tan"},
    {LBF_BIT32_ARSHIFT, "bit32.arshift"},
    {LBF_BIT32_BAND, "bit32.band"},
    {LBF_BIT32_BNOT, "bit32.bnot"},
    {LBF_BIT32_BOR, "bit32.bor"},
    {LBF_BIT32_BXOR, "bit32.bxor"},
    {LBF_BIT32_BTEST, "bit32.btest"},
    {LBF_BIT32_EXTRACT, "bit32.extract"},
    {LBF_BIT32_LROTATE, "bit32.lrotate"},
    {LBF_BIT32_LSHIFT, "bit32.lshift"},
    {LBF_BIT32_REPLACE, "bit32.replace"},
    {LBF_BIT32_RROTATE, "bit32.rrotate"},
    {LBF_BIT32_RSHIFT, "bit32.rshift"},
    {LBF_TYPE, "type"},
    {LBF_STRING_BYTE, "string.byte"},
    {LBF_STRING_CHAR, "string.char"},
    {LBF_STRING_LEN, "string.len"},
    {LBF_TYPEOF, "typeof"},
    {LBF_STRING_SUB, "string.sub"},
    {LBF_MATH_CLAMP, "math.clamp"},
    {LBF_MATH_SIGN, "math.sign"},
    {LBF_MATH_ROUND, "math.round"},
    {LBF_RAWSET, "rawset"},
    {LBF_RAWGET, "rawget"},
    {LBF_RAWEQUAL, "rawequal"},
    {LBF_TABLE_INSERT, "table.insert"},
    {LBF_TABLE_UNPACK, "table.unpack"},
    {LBF_VECTOR, "vector"},
    {LBF_BIT32_COUNTLZ, "bit32.countlz"},
    {LBF_BIT32_COUNTRZ, "bit32.countrz"},
    {LBF_SELECT_VARARG, "select"},
    {LBF_RAWLEN, "rawlen"},
    {LBF_BIT32_EXTRACTK, "bit32.extract"},
    {LBF_GETMETATABLE, "getmetatable"},
    {LBF_SETMETATABLE, "setmetatable"},

    // tonumber/tostring
    {LBF_TONUMBER, "tonumber"},
    {LBF_TOSTRING, "tostring"},

    // bit32.byteswap(n)
    {LBF_BIT32_BYTESWAP, "bit32.byteswap"},

    // buffer.
    {LBF_BUFFER_READI8, "buffer.readi8"},
    {LBF_BUFFER_READU8, "buffer.readu8"},
    {LBF_BUFFER_WRITEU8, "buffer.writeu8"},
    {LBF_BUFFER_READI16, "buffer.readi16"},
    {LBF_BUFFER_READU16, "buffer.readu16"},
    {LBF_BUFFER_WRITEU16, "buffer.writeu16"},
    {LBF_BUFFER_READI32, "buffer.readi32"},
    {LBF_BUFFER_READU32, "buffer.readu32"},
    {LBF_BUFFER_WRITEU32, "buffer.writeu32"},
    {LBF_BUFFER_READF32, "buffer.readf32"},
    {LBF_BUFFER_WRITEF32, "buffer.writef32"},
    {LBF_BUFFER_READF64, "buffer.readf64"},
    {LBF_BUFFER_WRITEF64, "buffer.writef64"},

    // vector.
    {LBF_VECTOR_MAGNITUDE, "vector.magnitude"},
    {LBF_VECTOR_NORMALIZE, "vector.normalize"},
    {LBF_VECTOR_CROSS, "vector.cross"},
    {LBF_VECTOR_DOT, "vector.dot"},
    {LBF_VECTOR_FLOOR, "vector.floor"},
    {LBF_VECTOR_CEIL, "vector.ceil"},
    {LBF_VECTOR_ABS, "vector.abs"},
    {LBF_VECTOR_SIGN, "vector.sign"},
    {LBF_VECTOR_CLAMP, "vector.clamp"},
    {LBF_VECTOR_MIN, "vector.min"},
    {LBF_VECTOR_MAX, "vector.max"},

    // math.lerp
    {LBF_MATH_LERP, "math.lerp"},

    // vector.lerp
    {LBF_VECTOR_LERP, "vector.lerp"},

    // math.
    {LBF_MATH_ISNAN, "math.isnan"},
    {LBF_MATH_ISINF, "math.isinf"},
    {LBF_MATH_ISFINITE, "math.isfinite"},

    /*// integer
    {LBF_INTEGER_CREATE, "integer.create"},
    {LBF_INTEGER_TONUMBER, "integer.tonumber"},
    {LBF_INTEGER_NEG, "integer.neg"},
    {LBF_INTEGER_ADD, "integer.add"},
    {LBF_INTEGER_SUB, "integer.sub"},
    {LBF_INTEGER_MUL, "integer.mul"},
    {LBF_INTEGER_DIV, "integer.div"},
    {LBF_INTEGER_MIN, "integer.min"},
    {LBF_INTEGER_MAX, "integer.max"},
    {LBF_INTEGER_REM, "integer.rem"},
    {LBF_INTEGER_IDIV, "integer.idiv"},
    {LBF_INTEGER_UDIV, "integer.udiv"},
    {LBF_INTEGER_UREM, "integer.urem"},
    {LBF_INTEGER_MOD, "integer.mod"},
    {LBF_INTEGER_CLAMP, "integer.clamp"},
    {LBF_INTEGER_BAND, "integer.band"},
    {LBF_INTEGER_BOR, "integer.bor"},
    {LBF_INTEGER_BNOT, "integer.bnot"},
    {LBF_INTEGER_BXOR, "integer.bxor"},
    {LBF_INTEGER_LT, "integer.lt"},
    {LBF_INTEGER_LE, "integer.le"},
    {LBF_INTEGER_ULT, "integer.ult"},
    {LBF_INTEGER_ULE, "integer.ule"},
    {LBF_INTEGER_GT, "integer.gt"},
    {LBF_INTEGER_GE, "integer.ge"},
    {LBF_INTEGER_UGT, "integer.ugt"},
    {LBF_INTEGER_UGE, "integer.uge"},
    {LBF_INTEGER_LSHIFT, "integer.lshift"},
    {LBF_INTEGER_RSHIFT, "integer.rshift"},
    {LBF_INTEGER_ARSHIFT, "integer.arshift"},
    {LBF_INTEGER_LROTATE, "integer.lrotate"},
    {LBF_INTEGER_RROTATE, "integer.rrotate"},
    {LBF_INTEGER_EXTRACT, "integer.extract"},
    {LBF_INTEGER_BTEST, "integer.btest"},
    {LBF_INTEGER_COUNTRZ, "integer.countrz"},
    {LBF_INTEGER_COUNTLZ, "integer.countlz"},
    {LBF_INTEGER_BSWAP, "integer.byteswap"},

    // buffer.readinteger / buffer.writeinteger (int64_t)
    {LBF_BUFFER_READINTEGER, "buffer.readinteger"},
    {LBF_BUFFER_WRITEINTEGER, "buffer.writeinteger"},*/
};

void Disassembler::write(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer) - 1, format, args);
    va_end(args);

    this->stream << buffer;
}

void Disassembler::decodeInstructions(Proto* p) {
    if (this->shouldDecode) {
        for (int i = 0; i < p->sizecode; i++) {
            std::uint32_t instruction = p->code[i];
            std::uint8_t opcode = LUAU_INSN_OP(instruction);

            auto it = std::find(this->opcodeDecodeTable.begin(),
                                this->opcodeDecodeTable.end(), opcode);

            std::uint8_t newOpcode;

            if (it != this->opcodeDecodeTable.end()) {
                newOpcode = it - this->opcodeDecodeTable.begin();
            } else {
                newOpcode = opcode;
            }

            std::uint32_t newInstruction;

            // TODO: when bytecode version goes past 6, or the latest few come
            // out of experimental, add their opcodes here

            // replace the instruction by each type
            switch (newOpcode) {
                // ABC
                case LOP_LOADNIL:
                case LOP_LOADB:
                case LOP_MOVE:
                case LOP_GETGLOBAL:
                case LOP_SETGLOBAL:
                case LOP_GETUPVAL:
                case LOP_SETUPVAL:
                case LOP_CLOSEUPVALS:
                case LOP_GETTABLE:
                case LOP_SETTABLE:
                case LOP_GETTABLEKS:
                case LOP_SETTABLEKS:
                case LOP_GETTABLEN:
                case LOP_SETTABLEN:
                case LOP_NAMECALL:
                case LOP_CALL:
                case LOP_RETURN:
                case LOP_ADD:
                case LOP_SUB:
                case LOP_MUL:
                case LOP_DIV:
                case LOP_MOD:
                case LOP_POW:
                case LOP_ADDK:
                case LOP_SUBK:
                case LOP_MULK:
                case LOP_DIVK:
                case LOP_MODK:
                case LOP_POWK:
                case LOP_AND:
                case LOP_OR:
                case LOP_ANDK:
                case LOP_ORK:
                case LOP_CONCAT:
                case LOP_NOT:
                case LOP_MINUS:
                case LOP_LENGTH:
                case LOP_NEWTABLE:
                case LOP_SETLIST:
                case LOP_FASTCALL3:
                case LOP_GETVARARGS:
                case LOP_PREPVARARGS:
                case LOP_FASTCALL:
                case LOP_CAPTURE:
                case LOP_SUBRK:
                case LOP_DIVRK:
                case LOP_FASTCALL1:
                case LOP_FASTCALL2:
                case LOP_FASTCALL2K: {
                    newInstruction = std::uint32_t(newOpcode) |
                                     (LUAU_INSN_A(instruction) << 8) |
                                     (LUAU_INSN_B(instruction) << 16) |
                                     (LUAU_INSN_C(instruction) << 24);
                    break;
                }
                // AD
                case LOP_LOADN:
                case LOP_LOADK:
                case LOP_GETIMPORT:
                case LOP_NEWCLOSURE:
                case LOP_JUMP:
                case LOP_JUMPBACK:
                case LOP_JUMPIF:
                case LOP_JUMPIFNOT:
                case LOP_JUMPIFEQ:
                case LOP_JUMPIFLE:
                case LOP_JUMPIFLT:
                case LOP_JUMPIFNOTEQ:
                case LOP_JUMPIFNOTLE:
                case LOP_JUMPIFNOTLT:
                case LOP_DUPTABLE:
                case LOP_FORNPREP:
                case LOP_FORNLOOP:
                case LOP_FORGLOOP:
                case LOP_FORGPREP_INEXT:
                case LOP_FORGPREP_NEXT:
                case LOP_DUPCLOSURE:
                case LOP_LOADKX:
                case LOP_FORGPREP:
                case LOP_JUMPXEQKNIL:
                case LOP_JUMPXEQKB:
                case LOP_JUMPXEQKN:
                case LOP_JUMPXEQKS:
                case LOP_IDIV:
                case LOP_IDIVK: {
                    newInstruction = std::uint32_t(newOpcode) |
                                     (LUAU_INSN_A(instruction) << 8) |
                                     (LUAU_INSN_D(instruction) << 16);
                    break;
                }
                // E
                case LOP_JUMPX:
                case LOP_COVERAGE: {
                    newInstruction = std::uint32_t(newOpcode) |
                                     (LUAU_INSN_E(instruction) << 8);
                    break;
                }
                default: {
                    newInstruction = instruction;
                    break;
                }
            }

            p->code[i] = newInstruction;

            // check if the opcode has an AUX afterwards
            switch (newOpcode) {
                case LOP_GETGLOBAL:
                case LOP_SETGLOBAL:
                case LOP_GETIMPORT:
                case LOP_GETTABLEKS:
                case LOP_SETTABLEKS:
                case LOP_NAMECALL:
                case LOP_JUMPIFEQ:
                case LOP_JUMPIFLE:
                case LOP_JUMPIFLT:
                case LOP_JUMPIFNOTEQ:
                case LOP_JUMPIFNOTLE:
                case LOP_JUMPIFNOTLT:
                case LOP_NEWTABLE:
                case LOP_SETLIST:
                case LOP_FASTCALL3:
                case LOP_FORGLOOP:
                case LOP_LOADKX:
                case LOP_FASTCALL2:
                case LOP_FASTCALL2K:
                case LOP_JUMPXEQKNIL:
                case LOP_JUMPXEQKB:
                case LOP_JUMPXEQKN:
                case LOP_JUMPXEQKS: {
                    i++;
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}

void Disassembler::processProto(Proto* p, Proto* parent) {
    this->decodeInstructions(p);

    const char* name = p->debugname != NULL ? p->debugname->data : "unknown";
    this->write("constants for %s:\n", name);
    this->write("size: %d\n", p->sizek);
    this->write("\tindex:\t\ttype:\t\tvalue:\n");
    for (int i = 0; i < p->sizek; i++) {
        this->write("\t%d\t\t", i);
        TValue* k = &p->k[i];
        this->write("%s\t\t", luaT_typenames[ttype(k)]);
        switch (ttype(k)) {
            case LUA_TBOOLEAN: {
                this->write("%s\n", bvalue(k) ? "true" : "false");
                break;
            }
            case LUA_TNUMBER: {
                this->write("%f\n", nvalue(k));
                break;
            }
            case LUA_TSTRING: {
                char* str = svalue(k);
                if (sizeof(str) > 50) {
                    strncpy(str, str, 50);
                }
                this->write("%s\n", str);
                break;
            }
            default: {
                this->write("\n");
                break;
            }
        }
    }

    if (p->is_vararg) {
        this->write("function %s(...)\n", name);
    } else {
        this->write("function %s(", name);
        for (int i = 1; i <= p->numparams; i++) {
            if (i == p->numparams) {
                this->write("arg%d", i);
            } else {
                this->write("arg%d, ", i);
            }
        }
        this->write(")\n");
    }

    for (int i = 0; i < p->sizecode; i++) {
        std::uint32_t insn = p->code[i];

        std::uint8_t opcode = LUAU_INSN_OP(insn);

        this->write("\t%s ", opcodeMap.find(opcode)->second.c_str());

        // TODO: parse each individual instruction's registers, ABC etc.
        switch (opcode) {
            case LOP_LOADNIL: {
                this->write("%d\n", LUAU_INSN_A(insn));
                break;
            }
            case LOP_LOADB: {
                this->write("%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn),
                            LUAU_INSN_C(insn));
                break;
            }
            case LOP_LOADN: {
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
                break;
            }
            case LOP_LOADK: {
                TValue* k = &p->k[LUAU_INSN_D(insn)];
                switch (ttype(k)) {
                    case LUA_TNUMBER: {
                        this->write("%d %d --> %f\n", LUAU_INSN_A(insn),
                                    LUAU_INSN_D(insn), nvalue(k));
                        break;
                    }
                    case LUA_TSTRING: {
                        this->write("%d %d --> '%s'\n", LUAU_INSN_A(insn),
                                    LUAU_INSN_D(insn), svalue(k));
                        break;
                    }
                }
                break;
            }
            case LOP_MOVE: {
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
                break;
            }
            case LOP_GETGLOBAL: {
                TValue* k = &p->k[p->code[++i]];
                this->write("%d %d --> '%s'\n", LUAU_INSN_A(insn), p->code[i],
                            svalue(k));
                break;
            }
            case LOP_SETGLOBAL: {
                TValue* k = &p->k[p->code[++i]];
                this->write("%d %d --> '%s'\n", LUAU_INSN_A(insn), p->code[i],
                            svalue(k));
                break;
            }
            case LOP_GETUPVAL:
            case LOP_SETUPVAL: {
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
                break;
            }
            case LOP_CLOSEUPVALS: {
                this->write("%d\n", LUAU_INSN_A(insn));
                break;
            }
            case LOP_GETIMPORT: {
                // TValue* k = &p->k[LUAU_INSN_D(insn) - 1];
                // this->write("%d %d --> '%s'\n", LUAU_INSN_A(insn),
                // LUAU_INSN_D(insn), svalue(k));
                std::uint32_t id = p->code[++i];
                int count = id >> 30;
                int id0 = int(id >> 20) & 1023;
                int id1 = int(id >> 10) & 1023;
                int id2 = int(id) & 1023;
                char buffer[1024];
                // this bit is the ugliest shit code you'll ever fucking see,
                // but it works!
                switch (count) {
                    case 1: {
                        strcpy(buffer, svalue(&p->k[id0]));
                        break;
                    }
                    case 2: {
                        strcpy(buffer, svalue(&p->k[id0]));
                        strcat(buffer, ".");
                        strcat(buffer, svalue(&p->k[id1]));
                        break;
                    }
                    case 3: {
                        strcpy(buffer, svalue(&p->k[id0]));
                        strcat(buffer, ".");
                        strcat(buffer, svalue(&p->k[id1]));
                        strcat(buffer, ".");
                        strcat(buffer, svalue(&p->k[id2]));
                        break;
                    }
                }
                this->write("%d %d %d --> '%s'\n", LUAU_INSN_A(insn),
                            LUAU_INSN_D(insn), id, buffer);
                break;
            }
            case LOP_GETTABLE:
            case LOP_SETTABLE: {
                this->write("%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn),
                            LUAU_INSN_C(insn));
                break;
            }
            case LOP_GETTABLEKS:
            case LOP_SETTABLEKS: {
                TValue* k = &p->k[p->code[++i]];
                this->write("%d %d %d --> '%s'\n", LUAU_INSN_A(insn),
                            LUAU_INSN_B(insn), LUAU_INSN_C(insn), svalue(k));
                break;
            }
            case LOP_GETTABLEN:
            case LOP_SETTABLEN: {
                this->write("%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn),
                            LUAU_INSN_C(insn));
                break;
            }
            case LOP_NEWCLOSURE: {
                Proto* p2 = p->p[LUAU_INSN_D(insn)];
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
                break;
            }
            case LOP_NAMECALL: {
                TValue* k = &p->k[p->code[++i]];
                this->write("%d %d %d --> '%s'\n", LUAU_INSN_A(insn),
                            LUAU_INSN_B(insn), LUAU_INSN_C(insn), svalue(k));
                break;
            }
            case LOP_CALL: {
                this->write("%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn),
                            LUAU_INSN_C(insn));
                break;
            }
            case LOP_RETURN: {
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
                break;
            }
            case LOP_JUMP:
            case LOP_JUMPBACK: {
                this->write("%d\n", LUAU_INSN_D(insn));
                break;
            }
            case LOP_JUMPIF:
            case LOP_JUMPIFNOT: {
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
                break;
            }
            case LOP_JUMPIFEQ:
            case LOP_JUMPIFLE:
            case LOP_JUMPIFLT:
            case LOP_JUMPIFNOTEQ:
            case LOP_JUMPIFNOTLE:
            case LOP_JUMPIFNOTLT: {
                this->write("%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_D(insn),
                            p->code[++i]);
                break;
            }
            case LOP_ADD:
            case LOP_SUB:
            case LOP_MUL:
            case LOP_DIV:
            case LOP_MOD:
            case LOP_POW: {
                this->write("%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn),
                            LUAU_INSN_C(insn));
                break;
            }
            case LOP_ADDK:
            case LOP_SUBK:
            case LOP_MULK:
            case LOP_DIVK:
            case LOP_MODK:
            case LOP_POWK: {
                TValue* k = &p->k[LUAU_INSN_C(insn)];
                this->write("%d %d %d --> ", LUAU_INSN_A(insn),
                            LUAU_INSN_B(insn));
                switch (ttype(k)) {
                    case LUA_TBOOLEAN: {
                        this->write("%s\n", bvalue(k) ? "true" : "false");
                        break;
                    }
                    case LUA_TNUMBER: {
                        this->write("%f\n", nvalue(k));
                        break;
                    }
                    case LUA_TSTRING: {
                        this->write("'%s'\n", svalue(k));
                        break;
                    }
                    default: {
                        this->write("%s\n", luaT_typenames[ttype(k)]);
                        break;
                    }
                }
                break;
            }
            case LOP_AND:
            case LOP_OR: {
                this->write("%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn),
                            LUAU_INSN_C(insn));
                break;
            }
            case LOP_ANDK:
            case LOP_ORK: {
                TValue* k = &p->k[LUAU_INSN_C(insn)];
                this->write("%d %d %d --> ", LUAU_INSN_A(insn),
                            LUAU_INSN_B(insn));
                switch (ttype(k)) {
                    case LUA_TBOOLEAN: {
                        this->write("%s\n", bvalue(k) ? "true" : "false");
                        break;
                    }
                    case LUA_TNUMBER: {
                        this->write("%f\n", nvalue(k));
                        break;
                    }
                    case LUA_TSTRING: {
                        this->write("'%s'\n", svalue(k));
                        break;
                    }
                    default: {
                        this->write("%s\n", luaT_typenames[ttype(k)]);
                        break;
                    }
                }
                break;
            }
            case LOP_CONCAT: {
                this->write("%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn),
                            LUAU_INSN_C(insn));
                break;
            }
            case LOP_NOT:
            case LOP_MINUS:
            case LOP_LENGTH: {
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
                break;
            }
            case LOP_NEWTABLE: {
                this->write("%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn),
                            p->code[++i]);
                break;
            }
            case LOP_DUPTABLE: {
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
                break;
            }
            case LOP_SETLIST: {
                this->write("%d %d %d %d\n", LUAU_INSN_A(insn),
                            LUAU_INSN_B(insn), LUAU_INSN_C(insn), p->code[++i]);
                break;
            }
            case LOP_FORNPREP:
            case LOP_FORNLOOP:
            case LOP_FORGPREP: {
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
                break;
            }
            case LOP_FORGLOOP: {
                this->write("%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_D(insn),
                            p->code[++i]);
                break;
            }
            case LOP_FORGPREP_INEXT:
            case LOP_FORGPREP_NEXT: {
                this->write("%d\n", LUAU_INSN_A(insn));
                break;
            }
            case LOP_GETVARARGS: {
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
            }
            case LOP_DUPCLOSURE: {
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
                break;
            }
            case LOP_PREPVARARGS: {
                this->write("%d\n", LUAU_INSN_A(insn));
                break;
            }
            case LOP_LOADKX: {
                TValue* k = &p->k[p->code[++i]];
                switch (ttype(k)) {
                    case LUA_TNUMBER: {
                        this->write("%d %d --> %f\n", LUAU_INSN_A(insn),
                                    LUAU_INSN_D(insn), nvalue(k));
                        break;
                    }
                    case LUA_TSTRING: {
                        this->write("%d %d --> '%s'\n", LUAU_INSN_A(insn),
                                    LUAU_INSN_D(insn), svalue(k));
                        break;
                    }
                }
                break;
            }
            case LOP_JUMPX: {
                this->write("%d\n", LUAU_INSN_E(insn));
                break;
            }
            case LOP_FASTCALL: {
                this->write(
                    "%d %d --> '%s'\n", LUAU_INSN_A(insn), LUAU_INSN_C(insn),
                    builtinFunctionMap.find(LUAU_INSN_A(insn))->second.c_str());
                break;
            }
            case LOP_COVERAGE: {
                this->write("%d\n", LUAU_INSN_E(insn));
            }
            case LOP_CAPTURE: {
                this->write("%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
                break;
            }
            case LOP_SUBRK:
            case LOP_DIVRK: {
                TValue* k = &p->k[LUAU_INSN_B(insn)];
                this->write("%d %d %d --> ", LUAU_INSN_A(insn),
                            LUAU_INSN_B(insn), LUAU_INSN_C(insn));
                switch (ttype(k)) {
                    case LUA_TBOOLEAN: {
                        this->write("%s\n", bvalue(k) ? "true" : "false");
                        break;
                    }
                    case LUA_TNUMBER: {
                        this->write("%f\n", nvalue(k));
                        break;
                    }
                    case LUA_TSTRING: {
                        this->write("'%s'\n", svalue(k));
                        break;
                    }
                    default: {
                        this->write("%s\n", luaT_typenames[ttype(k)]);
                        break;
                    }
                }
                break;
            }
            case LOP_FASTCALL1: {
                this->write(
                    "%d %d %d --> '%s'\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn),
                    LUAU_INSN_C(insn),
                    builtinFunctionMap.find(LUAU_INSN_A(insn))->second.c_str());
                break;
            }
            case LOP_FASTCALL2: {
                this->write(
                    "%d %d %d %d --> '%s'\n", LUAU_INSN_A(insn),
                    LUAU_INSN_B(insn), LUAU_INSN_C(insn), p->code[++i],
                    builtinFunctionMap.find(LUAU_INSN_A(insn))->second.c_str());
                break;
            }
            case LOP_FASTCALL3: {
                auto aux = p->code[++i];
                this->write(
                    "%d %d %d %d %d --> '%s'\n", LUAU_INSN_A(insn),
                    LUAU_INSN_B(insn), LUAU_INSN_C(insn), (aux & 0xFF),
                    ((aux >> 8) & 0xFF),
                    builtinFunctionMap.find(LUAU_INSN_A(insn))->second.c_str());
                break;
            }
            case LOP_FASTCALL2K: {
                TValue* k = &p->k[p->code[++i]];
                this->write(
                    "%d %d %d %d --> '%s' ", LUAU_INSN_A(insn),
                    LUAU_INSN_B(insn), LUAU_INSN_C(insn), p->code[i],
                    builtinFunctionMap.find(LUAU_INSN_A(insn))->second.c_str());
                switch (ttype(k)) {
                    case LUA_TBOOLEAN: {
                        this->write("%s\n", bvalue(k) ? "true" : "false");
                        break;
                    }
                    case LUA_TNUMBER: {
                        this->write("%f\n", nvalue(k));
                        break;
                    }
                    case LUA_TSTRING: {
                        this->write("'%s'\n", svalue(k));
                        break;
                    }
                    default: {
                        this->write("%s\n", luaT_typenames[ttype(k)]);
                        break;
                    }
                }
                break;
            }
            case LOP_JUMPXEQKNIL: {
                this->write("%d %d %d --> NOT: %s", LUAU_INSN_A(insn),
                            LUAU_INSN_D(insn), p->code[++i],
                            (p->code[i] >> 31) ? "true" : "false");
                break;
            }
            case LOP_JUMPXEQKB: {
                this->write("%d %d %d --> %s, NOT: %s", LUAU_INSN_A(insn),
                            LUAU_INSN_D(insn), p->code[++i],
                            (p->code[i] & 1) ? "true" : "false",
                            (p->code[i] >> 31) ? "true" : "false");
                break;
            }
            case LOP_JUMPXEQKN:
            case LOP_JUMPXEQKS: {
                TValue* k = &p->k[p->code[++i] & 0xffffff];
                this->write("%d %d %d --> ", LUAU_INSN_A(insn),
                            LUAU_INSN_D(insn), p->code[i]);
                switch (ttype(k)) {
                    case LUA_TBOOLEAN: {
                        this->write("%s", bvalue(k) ? "true" : "false");
                        break;
                    }
                    case LUA_TNUMBER: {
                        this->write("%f", nvalue(k));
                        break;
                    }
                    case LUA_TSTRING: {
                        this->write("'%s'", svalue(k));
                        break;
                    }
                    default: {
                        this->write("%s", luaT_typenames[ttype(k)]);
                        break;
                    }
                }
                this->write(", NOT: %s\n",
                            (p->code[i] >> 31) ? "true" : "false");
                break;
            }
            case LOP_IDIV: {
                this->write("%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn),
                            LUAU_INSN_C(insn));
                break;
            }
            case LOP_IDIVK: {
                TValue* k = &p->k[LUAU_INSN_C(insn)];
                this->write("%d %d %d --> ", LUAU_INSN_A(insn),
                            LUAU_INSN_B(insn), LUAU_INSN_C(insn));
                switch (ttype(k)) {
                    case LUA_TBOOLEAN: {
                        this->write("%s\n", bvalue(k) ? "true" : "false");
                        break;
                    }
                    case LUA_TNUMBER: {
                        this->write("%f\n", nvalue(k));
                        break;
                    }
                    case LUA_TSTRING: {
                        this->write("'%s'\n", svalue(k));
                        break;
                    }
                    default: {
                        this->write("%s\n", luaT_typenames[ttype(k)]);
                        break;
                    }
                }
                break;
            }
            default: {
                this->write("\n");
                break;
            }
        }
    }

    this->write("end\n\n");

    for (int i = 0; i < p->sizep; i++) {
        this->processProto(p->p[i], p);
    }
}

bool Disassembler::disassemble() {
    lua_State* L = luaL_newstate();

    luaL_sandboxthread(L);

    if (luau_load(L, "=Script", this->inputBytecode.c_str(),
                  this->inputBytecode.size(), 0)) {
        return false;
    }

    // our bytecode has now been parsed and we can traverse the instructions in
    // the protos it has created.
    Closure* mainClosure = clvalue(L->top - 1);

    mainClosure->l.p->debugname =
        luaS_newlstr(L, "main", 4);  // makes the main script's function be
                                     // called "main" instead of "(null)"

    this->processProto(mainClosure->l.p, nullptr);

    return true;
}

std::string Disassembler::getStreamData() { return this->stream.str(); }
