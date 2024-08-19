// ====================================================
// FILE:
//     InstrumentPrintFuncCall.h
//
// DESCRIPTION:
//     Header file that declares an InstrumentPrintFunc Pass 
//        
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#ifndef INSTRUMENT_PRINT_H
#define INSTRUMENT_PRINT_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

namespace instrumentprint {
struct InstrumentPrintFuncCall : public llvm::PassInfoMixin<InstrumentPrintFuncCall> {
  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &);
  bool runOnModule(llvm::Module &M);

  static bool isRequired() { return true; }
};

} // namespace instrumentprint

#endif
