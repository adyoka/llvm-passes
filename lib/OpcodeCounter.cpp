// ====================================================
// FILE:
//     OpcodeCounter.cpp
//
// DESCRIPTION:
//     An analysis pass that maps and counts how many times each LLVM IR opcode was used in each function
//     Printer Pass prints the output to stderr/os
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================


#include "OpcodeCounter.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace opcodecounter {

AnalysisKey OpcodeCounter::Key;

OpcodeCounter::Result OpcodeCounter::run(Function &F,FunctionAnalysisManager &) {
    OpcodeCounter::Result OpcodeMap;

    for (auto &BB : F) {
    for (auto &Inst : BB) {
        StringRef Name = Inst.getOpcodeName();

        if (OpcodeMap.find(Name) == OpcodeMap.end()) {
            OpcodeMap[Inst.getOpcodeName()] = 1;
        } else {
            OpcodeMap[Inst.getOpcodeName()]++;
        }
    }
    }

    return OpcodeMap;
}

PreservedAnalyses OpcodeCounterPrinter::run(Function &F, FunctionAnalysisManager &FAM) {
    auto &OpcodeMap = FAM.getResult<OpcodeCounter>(F);

    OS << "Printing analysis 'OpcodeCounter Pass' for function '"<< F.getName() << "':\n";
    OS << "=================================================\n";
    const char *str1 = "OPCODE";
    const char *str2 = "#TIMES USED";
    OS << format("%-15s %-10s\n", str1, str2);
    OS << "-------------------------------------------------\n";
    for (auto &opcode : OpcodeMap) {
        OS << format("%-15s %-10lu\n", opcode.first().str().c_str(),opcode.second);
    }
    OS << "-------------------------------------------------\n";

    return PreservedAnalyses::all();
}

} // namespace opcodecounter



// -------------------------------------------------
// Pass registration as plugins in new PM interface
// -------------------------------------------------


PassPluginLibraryInfo getOpcodeCounterPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "OpcodeCounter", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "print<opcode-counter>") {
                        FPM.addPass(opcodecounter::OpcodeCounterPrinter(errs()));
                        return true;
                    }
                    if (Name == "opcode-counter") {
                        FPM.addPass(opcodecounter::OpcodeCounter());
                        return true;
                    }
                    return false;
                }
            );

            // registering analysis pass so that others PM can invoke getResult
            PB.registerAnalysisRegistrationCallback(
                [](FunctionAnalysisManager &FAM) {
                    FAM.registerPass([&] { return opcodecounter::OpcodeCounter(); } );
                }
            );
        }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getOpcodeCounterPluginInfo();
}

