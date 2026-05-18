target_sources(luau-disasm PRIVATE
    src/main.cpp
    src/ArgParser.cpp
    src/Compiler.cpp
    src/Disassembler.cpp

    # luau/VM/src/lapi.h
    # luau/VM/src/lbuffer.h
    # luau/VM/src/lbuiltins.h
    # luau/VM/src/lbytecode.h
    # luau/VM/src/lclass.h
    # luau/VM/src/lcommon.h
    # luau/VM/src/ldebug.h
    # luau/VM/src/ldo.h
    # luau/VM/src/lfunc.h
    # luau/VM/src/lgc.h
    # luau/VM/src/lmem.h
    # luau/VM/src/lnumutils.h
    # luau/VM/src/lobject.h
    # luau/VM/src/lstate.h
    # luau/VM/src/lstring.h
    # luau/VM/src/ltable.h
    # luau/VM/src/ltm.h
    # luau/VM/src/ludata.h
    # luau/VM/src/lvm.h
)

target_include_directories(luau-disasm PRIVATE
    include/
    luau/VM/include/
    luau/VM/src/
)