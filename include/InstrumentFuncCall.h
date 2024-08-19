// ====================================================
// FILE:
//     InstrumentFuncCall.h
//
// DESCRIPTION:
//     Header file that declares an InstrumentFuncCall Pass 
//        
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#ifndef INSTRUMENT_BASIC_H
#define INSTRUMENT_BASIC_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

struct InstrumentFuncCall : public llvm::PassInfoMixin<InjectFuncCall> {
  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &);
  bool runOnModule(llvm::Module &M);
  
  static bool isRequired() { return true; }
};


#endif
