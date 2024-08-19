// ====================================================
// FILE:
//     InstrumentFuncCall.cpp
//
// DESCRIPTION:
//     A simple transformation pass that inserts a call to printf for each defined function of the LLVM IR module
//     
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================


#include "InstrumentFuncCall.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

using namespace llvm;

namespace instrumentbasic {

bool InstrumentFuncCall::runOnModule(Module &M) {
    bool Changed = false;

    auto &CTX = M.getContext();
    
}

PreservedAnalyses InstrumentFuncCall::run(Module &M, ModuleAnalysisManager &) {
  bool Changed =  runOnModule(M);

  return (Changed ? llvm::PreservedAnalyses::none() : llvm::PreservedAnalyses::all());
}

} //namespace instrumentbasic