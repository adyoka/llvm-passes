// ====================================================
// FILE:
//     DynamicCallCounterl.h
//
// DESCRIPTION:
//     Header file that declares the Pass that prints dynamic function calls during run-time.
//        
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#ifndef INSTRUMENT_DYNAMIC_CALL_H
#define INSTRUMENT_DYNAMIC_CALL_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

namespace dynamicallc {

struct DynamicCallCounter : public llvm::PassInfoMixin<DynamicCallCounter> {
  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &);
  bool runOnModule(llvm::Module &M);
  
  static bool isRequired() { return true; }
};

}

#endif