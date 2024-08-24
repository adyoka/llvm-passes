// ====================================================
// FILE:
//     ArithmeticObfuscators.cpp
//
// DESCRIPTION:
//     A pass that obfuscates code of the simple arithmetic operations by transforming it to a Mixed-Boolean Arithmetic (MBA) expression:
//     1. Integer subtraction:
//        a - b transforms to (a + ~b) + 1
//     2. 8-bit integer addition:
//        a + b transforms to (((a ^ b) + 2 * (a & b)) * 39 + 23) * 151 + 111
//
//     Formula from: llvm-tutor
//     Original source: "Defeating MBA-based Obfuscation" Ninon Eyrolles, Louis Goubin, Marion Videau
//      
//     
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#include "ArithmeticObfuscators.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"


using namespace llvm;

namespace obfuscator {

bool MBASubTransform::runOnBasicBlock(BasicBlock& BB) {
  bool Changed = false;

  for (auto Instr = BB.begin(), IEnd = BB.end(); Instr != IEnd; ++Instr) {
    // skip if the instruction is not a binary operator
    auto *BinOp = dyn_cast<BinaryOperator>(Instr);
    if (BinOp == nullptr) continue;

    // skip if operation is not a Subtraction and not returning integer
    auto Opcode = BinOp->getOpcode();
    if (Opcode != Instruction::Sub || !BinOp->getType()->isIntegerTy()) continue;

    IRBuilder<> Builder(BinOp);

    // create new instruction (a + ~b) + 1
    Instruction *NewInstr= BinaryOperator::CreateAdd(
      Builder.CreateAdd( BinOp->getOperand(0), Builder.CreateNot(BinOp->getOperand(1)) ),
      ConstantInt::get(BinOp->getType(), 1)
    );

    ReplaceInstWithInst(&BB, Instr, NewInstr);

    Changed = true;

  }
  return Changed;
}

PreservedAnalyses MBASubTransform::run(Function &F, FunctionAnalysisManager &) {
  bool Changed = false;

  for (auto &BB : F) {
    Changed |= runOnBasicBlock(BB);
  }
  return (Changed ? PreservedAnalyses::none() : PreservedAnalyses::all());
}


bool MBAAddTransform::runOnBasicBlock(BasicBlock& BB) {
  bool Changed = false;

  for (auto Instr = BB.begin(), IEnd = BB.end(); Instr != IEnd; ++Instr) {
    // skip if the instruction is not a binary operator
    auto *BinOp = dyn_cast<BinaryOperator>(Instr);
    if (BinOp == nullptr) continue;

    // skip if operation is not a Subtraction and not returning integer
    auto Opcode = BinOp->getOpcode();
    if (Opcode != Instruction::Add) continue;

    // skip if operands are not type of i8
    if (!BinOp->getType()->isIntegerTy() || !(BinOp->getType()->getIntegerBitWidth() == 8)) continue;


    IRBuilder<> Builder(BinOp);

    // create new instruction (((a ^ b) + 2 * (a & b)) * 39 + 23) * 151 + 111
    auto Const2 = ConstantInt::get(BinOp->getType(), 2);
    auto Const39 = ConstantInt::get(BinOp->getType(), 39);
    auto Const23 = ConstantInt::get(BinOp->getType(), 23);
    auto Const151 = ConstantInt::get(BinOp->getType(), 151);
    auto Const111 = ConstantInt::get(BinOp->getType(), 111);
    Instruction *NewInstr= BinaryOperator::CreateAdd(
      Const111, 
      Builder.CreateMul(
        Const151,
        Builder.CreateAdd(
          Const23,
          Builder.CreateMul(
            Const39, 
            Builder.CreateAdd(
              Builder.CreateXor(BinOp->getOperand(0), BinOp->getOperand(1)),
              Builder.CreateMul(
                Const2,
                Builder.CreateAnd(BinOp->getOperand(0), BinOp->getOperand(1))
              )
            )
          )
        )
      )
    );

    ReplaceInstWithInst(&BB, Instr, NewInstr);

    Changed = true;

  }
  return Changed;
}


PreservedAnalyses MBAAddTransform::run(Function &F, FunctionAnalysisManager &) {
  bool Changed = false;

  for (auto &BB : F) {
    Changed |= runOnBasicBlock(BB);
  }
  return (Changed ? PreservedAnalyses::none() : PreservedAnalyses::all());
}


} // namespace obfuscator



// -------------------------------------------------
// Pass registration as plugins in new PM interface
// -------------------------------------------------

llvm::PassPluginLibraryInfo getArithObfuscatorsPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "arith-obfuscators", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "mba-sub") {
                    FPM.addPass(MBASubTransform());
                    return true;
                  }
                  else if (Name == "mba-add") {
                    FPM.addPass(MBAAddTransform());
                    return true;
                  }
                  return false;
                });
          }};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getArithObfuscatorsPluginInfo();
}