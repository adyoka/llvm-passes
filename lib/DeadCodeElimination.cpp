// ====================================================
// FILE:
//     DeadCodeElimination.cpp
//
// DESCRIPTION:
//     
//      
//     
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#include "DeadCodeElimination.h"

#include "llvm/ADT/Statistic.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/AssumeBundleBuilder.h"

#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace dce {

static bool checkInstForDCE (Instruction *I, SmallSetVector<Instruction *, 16> &InstSet, const TargetLibraryInfo *TLI) {
    bool Changed = false;
    if (isInstructionTriviallyDead(I, TLI)) {
        salvageDebugInfo(*I);
        salvageKnowledge(I);

        for (unsigned it = 0, e = I->getNumOperands(); it != e; ++it) {
            Value* OpVal = I->getOperand(it);
            I->setOperand(it, nullptr);

            if (!OpVal->use_empty() || I == OpVal) continue;

            Instruction *OpInst = dyn_cast<Instruction>(OpVal);

            if (OpInst && isInstructionTriviallyDead(OpInst, TLI)) {
                InstSet.insert(OpInst);
            }

        }

        I->eraseFromParent();
        
        Changed = true;
    }

    return Changed;
}

static bool ScanAndEliminateDeadCode(Function &F, const TargetLibraryInfo *TLI) {
    bool Changed = false;

    SmallSetVector<Instruction *, 16> InstSet;
    for (Instruction &I : make_early_inc_range(instructions(F))) {
        if (!InstSet.contains(&I)) {
            Changed |= checkInstForDCE(&I, InstSet, TLI);
        }
    }


}

PreservedAnalyses DeadCodeElimPass::run(Function &F, FunctionAnalysisManager &FAM) {
    TargetLibraryInfo TLI = FAM.getResult<TargetLibraryAnalysis>(F);
    if (!ScanAndEliminateDeadCode(F, &TLI))
        return PreservedAnalyses::all();

    PreservedAnalyses PA;
    PA.preserveSet<CFGAnalyses>();
    return PA;
}


} // namespace dce


// -------------------------------------------------
// Pass registration as plugins in new PM interface
// -------------------------------------------------


llvm::PassPluginLibraryInfo getDCEPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "dce-pass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "dce-pass") {
                    FPM.addPass(DeadCodeElimPass());
                    return true;
                  }
                  return false;
                });
          }};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getDCEPassPluginInfo();
}