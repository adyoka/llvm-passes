# LLVM Passes and Tools

A collection of analysis and transformation LLVM-based passes and tools.

* **Out-of-tree** passes - implemented out of the LLVM source tree and built against the binary installation
* Based on the latest **LLVM 18** version

### Overview

A pass is a program that is applied on a target program in order to transform, optimize, obfuscate, profile it, and many other reasons. Sometimes, a pass may require some analysis information - other passes may provide it. Hence, a pass can be a transformation pass that changes the (IR) code or an analysis pass that returns analysis results. 

LLVM applies a chain of transformation and analyses on the input program to optimize it under different optimization levels and has an extensive [list](https://llvm.org/docs/Passes.html) of passes implemented in its source tree.

This project implements custom out-of-tree analysis and transformation passes and LLVM-based command line tools (on the base of these passes). These passes are simple but can be used to instrument code, optimize to some level, or obfuscate (for security reasons). Mostly, these passes were implemented to learn more about LLVM (thanks llvm-tutor), optimizations, and "middle-end" of the compilers.

### Passes

Implemented passes:

| Name                                             | Description                                                                                       |
|--------------------------------------------------|---------------------------------------------------------------------------------------------------|
| [**OpcodeCounter**](lib/OpcodeCounter.cpp)       | Conducts an analysis and prints a summary of LLVM IR opcodes in the input file                    |
| [**InstrumentFuncCall**](lib/InstrumentFuncCall.cpp) | Prints function info for every function defined in the module by instrumenting `printf` call        |
| [**StaticCallCounter**](lib/StaticCallCounter.cpp) | Counts direct (static) function calls at compile-time                                              |
| [**DynamicCallCounter**](lib/DynamicCallCounter.cpp) | Counts direct (dynamic) function calls at run-time                                                 |
| [**ArithmeticObfuscators**](lib/ArithmeticObfuscators.cpp) | Obfuscates integer `sub` and 8-bit integer `add` instructions by transforming into equal Mixed-Boolean-Arithmetic expressions |
| [**DeadCodeElimination**](lib/DeadCodeElimination.cpp) | Removes obviously dead instructions and newly dead code after the transformation              |


More passes are being added along the way.

### Build

To build the project, you first need to intsall and build LLVM:

`brew install llvm`

Or follow the official tutorial on building from source.

Build all passes:

```bash
mkdir build
LLVM_INSTALL_DIR=<installation/dir/of/llvm>
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_INSTALL_DIR -G "Ninja" -B build 
cmkae --build .
```

### Run

```bash
# Generate an input LLVM IR file (.ll)
clang -emit-llvm -S ./inputs/input_file.c -o input_file.ll
# Run the pass through opt
opt -load-pass-plugin ./lib/<libPassName>.dylib --passes="pass-name" input_file.ll
```

The names for the pipeline `--passes=` are:
* `print<opcode-counter>`       - for OpcodeCounter pass
* `instrument-print-func-call`  - for InstrumentFuncCall pass
* `print<static-cc>`            - for StaticCallCounter pass 
* `dynamic-cc`                  - for DynamicCallCounter pass
* `dce-pass`                    - for DeadCodeElimination pass
* `mba-sub`                     - for ArithmeticObfuscatorSub pass
* `mba-add`                     - for ArithmeticObfuscatorAdd pass

Running standalone tools (dce, static_cc):

```bash
./bin/dce input.cc
./bin/static_cc input.cc
```

#### Implementation

You can find the implementation of the pass plugins in .cpp files inside `lib` directory and declaration in .h files insde 'include'.

The implementation of the standalone tools are in 'tools' directory and their binary executables in 'bin'.

Some input files are provided and their corresponding IR files are in 'examples' directory.


