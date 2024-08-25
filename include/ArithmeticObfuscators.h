// ====================================================
// FILE:
//     ArithmeticObfuscators.h
//
// DESCRIPTION:
//     Header file that declares the Pass that obfuscates simple arithmetic operations.
//        
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================


#ifndef ARITHMETIC_OBFUSCATOR_H
#define ARITHMETIC_OBFUSCATOR_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

namespace obfuscator {

struct MBASubTransform : public llvm::PassInfoMixin<MBASubTransform> {
  llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &);
  bool runOnBasicBlock(llvm::BasicBlock &BB);

  static bool isRequired() { return true; }
};

struct MBAAddTransform : public llvm::PassInfoMixin<MBAAddTransform> {
  llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &);
  bool runOnBasicBlock(llvm::BasicBlock &BB);

  static bool isRequired() { return true; }
};

}
#endif