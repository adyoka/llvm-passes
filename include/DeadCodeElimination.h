// ====================================================
// FILE:
//     DeadCodeElimination.h
//
// DESCRIPTION:
//     Header file that declares the Pass that eliminates dead instructions and newly dead code
//        
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#ifndef DCE_H
#define DCE_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

namespace dce {

class DeadCodeElimPass : public llvm::PassInfoMixin<DeadCodeElimPass> {
public:
    llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM);
    static bool isRequired() { return true; }
};

}

#endif