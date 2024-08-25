// ====================================================
// FILE:
//     DeadCodeElimination.cpp
//
// DESCRIPTION:
//     The pass removes all the (obviously) dead instructions,
//     and then removes any newly dead code by rechecking instructions used by removed ones.
//      
//     
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#include "DeadCodeElimination.h"

#include "llvm/ADT/Statistic.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/InstIterator.h" // make_early_inc_range(instructions(F))
#include "llvm/Transforms/Utils/Local.h" // isInstructionTriviallyDead()
#include "llvm/Transforms/Utils/AssumeBundleBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#define DEBUG_TYPE "dce-pass"

using namespace llvm;

namespace dce {

STATISTIC(DeadCodeEliminated, "The # of dead instructions deleted");

static bool checkInstForDCE (Instruction *I, SmallSetVector<Instruction *, 16> &InstSet, const TargetLibraryInfo *TLI) {
    bool Changed = false;

    if (isInstructionTriviallyDead(I, TLI)) { 
        salvageDebugInfo(*I);
        salvageKnowledge(I);

        // iterate through I's operands to null out
        for (unsigned it = 0, e = I->getNumOperands(); it != e; ++it) {
            Value* OpVal = I->getOperand(it);
            I->setOperand(it, nullptr);

            if (!OpVal->use_empty() || I == OpVal) continue;

            // if the operand became trivially dead code after we nulled out it, 
            // add it to the set for future deletion 
            Instruction *OpInst = dyn_cast<Instruction>(OpVal);
            if (OpInst && isInstructionTriviallyDead(OpInst, TLI)) {
                InstSet.insert(OpInst);
            }

        }

        I->eraseFromParent();
        ++DeadCodeEliminated;

        Changed = true;
    }

    return Changed;
}

static bool ScanAndEliminateDeadCode(Function &F, const TargetLibraryInfo *TLI) {
    bool Changed = false;

    SmallSetVector<Instruction *, 16> InstSet; // set of instructions to check for DCE

    // iterate through insructions of F but in early increment way, 
    // so that isntruction deletions would not affect iteration
    for (Instruction &I : make_early_inc_range(instructions(F))) {
        if (!InstSet.contains(&I)) {
            Changed |= checkInstForDCE(&I, InstSet, TLI);
        }
    }

    // checking potentially newly dead instructions
    while (!InstSet.empty()) {
        Instruction *I = InstSet.pop_back_val();
        Changed |= checkInstForDCE(I, InstSet, TLI);
    }
    return Changed;
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
                    FPM.addPass(dce::DeadCodeElimPass());
                    return true;
                  }
                  return false;
                });
          }};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getDCEPassPluginInfo();
}