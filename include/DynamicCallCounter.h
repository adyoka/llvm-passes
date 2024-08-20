// ====================================================
// FILE:
//     DynamicCallCounterl.h
//
// DESCRIPTION:
//     
//        
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#ifndef INSTRUMENT_DYNAMIC_CALL_H
#define INSTRUMENT_DYNAMIC_CALL_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

struct DynamicCallCounter : public llvm::PassInfoMixin<DynamicCallCounter> {
  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &);
  bool runOnModule(llvm::Module &M);
  
  static bool isRequired() { return true; }
};

#endif