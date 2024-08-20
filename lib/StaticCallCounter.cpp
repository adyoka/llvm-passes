// ====================================================
// FILE:
//     StaticCallCounter.cpp
//
// DESCRIPTION:
//     
//     Printer Pass prints the output to stderr/os
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#include "StaticCallCounter.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace staticcallc {

StaticCallCounter::Result StaticCallCounter::run(Module &M, ModuleAnalysisManager &) {
    MapVector<const Function *, unsigned> resultMap;

    for (auto &Func : M) {
        for (auto &BB : Func) {
            for (auto &Instr : BB) {

                // check if Intrs is call base
                auto *CBInstr = dyn_cast<CallBase>(&Instr);
                // we need only CallBase instructions
                if (CBInstr == nullptr) continue;

                // check for direct call
                auto DirectCall = CBInstr->getCalledFunction();
                // we need only direct invocation of a function
                if (DirectCall == nullptr) continue;

                // find/insert in Map and increment its count
                auto callCount = resultMap.find(DirectCall);
                if (callCount == resultMap.end()) {
                    callCount = resultMap.insert(std::make_pair(DirectCall, 0)).first;
                }
                callCount->second++;
            }
        }
    }

    return resultMap;
}

PreservedAnalyses StaticCallCounterPrinter::run(Module &M,ModuleAnalysisManager &MAM) {
    auto CallMap = MAM.getResult<StaticCallCounter>(M);

    OS << "=================================================\n";
    OS << "Static Call Counter analysis results: \n";
    OS << "=================================================\n";
    const char *str1 = "NAME";
    const char *str2 = "# OF STATIC DIRECT CALLS";
    OS << format("%-20s %-10s\n", str1, str2);
    OS << "-------------------------------------------------\n";

    for (auto &CallCount : CallMap) {
        OS << format("%-20s %-10lu\n", CallCount.first->getName().str().c_str(), CallCount.second);
    }

    OS << "-------------------------------------------------\n";
}


} // namespace staticcallc


// -------------------------------------------------
// Pass registration as plugins in new PM interface
// -------------------------------------------------


AnalysisKey staticcallc::StaticCallCounter::Key;

llvm::PassPluginLibraryInfo getStaticCallCounterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "static-cc", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "print<static-cc>") {
                    MPM.addPass(staticcallc::StaticCallCounterPrinter(llvm::errs()));
                    return true;
                  }
                  if (Name == "static-cc") {
                    MPM.addPass(staticcallc::StaticCallCounter());
                    return true;
                  }
                  return false;
                });
            // registering analysis pass so that others PM can invoke getResult
            PB.registerAnalysisRegistrationCallback(
                [](ModuleAnalysisManager &MAM) {
                  MAM.registerPass([&] { return staticcallc::StaticCallCounter(); });
                });
          }};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getStaticCallCounterPluginInfo();
}

