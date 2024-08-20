// ====================================================
// FILE:
//     DynamicCallCounter.cpp
//
// DESCRIPTION:
//     
//
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#include "DynamicCallCounter.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace dynamicallc {

bool DynamicCallCounter::runOnModule(Module &M) {
    bool Changed = false;

    StringMap<Constant *> CallCountMap;
    StringMap<Constant *> FuncNameMap;
    
    auto &CTX = M.getContext();

    // traverse each function in the module and instrument call counter
    for (auto &F : M) {
        if (F.isDeclaration()) continue;

        // builder for instrumentation of code at the beginning of each function
        IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());

        // variable for the counter of function calls
        std::string counterVarName = "counter_" + std::string(F.getName());
        Constant *GlobalVar = M.getOrInsertGlobal(counterVarName, IntegerType::getInt32Ty(CTX));

        // get the variable by var name, align and init to unsigned int 0
        GlobalVariable *GV = M.getNamedGlobal(counterVarName);
        GV->setLinkage(GlobalValue::CommonLinkage);
        GV->setAlignment(MaybeAlign(4));
        GV->setInitializer(ConstantInt::get(CTX, APInt(32, 0)));

        CallCountMap[F.getName()] = GlobalVar;

        



    }

    return Changed;
}

PreservedAnalyses DynamicCallCounter::run(Module &M, ModuleAnalysisManager &) {
  bool Changed = runOnModule(M);

  return (Changed ? PreservedAnalyses::none() : PreservedAnalyses::all());
}


} // namespace dynamicallc


llvm::PassPluginLibraryInfo getDynamicCallCounterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "static-cc", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, ModulePassManager &MPM, ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "dynamic-cc") {
                    MPM.addPass(DynamicCallCounter());
                    return true;
                  }
                  return false;
                });
          }};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getDynamicCallCounterPluginInfo();
}