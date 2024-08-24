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

class DeadCodeElimPass : public PassInfoMixin<DeadCodeElimPass> {
    llvm::PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);

    static bool isRequired() { return true; }
};

#endif