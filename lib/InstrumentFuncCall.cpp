// ====================================================
// FILE:
//     InstrumentPrintFuncCall.cpp
//
// DESCRIPTION:
//     A simple transformation pass that inserts a call to printf for each defined function of the LLVM IR module
//     
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================


#include "InstrumentFuncCall.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

using namespace llvm;

namespace instrumentprint {

bool InstrumentPrintFuncCall::runOnModule(Module &M) {
    bool Changed = false;

    auto &CTX = M.getContext();
    // declare i32 @printf(i8*, ...)
    // i8* = Int8Ty:
    PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(CTX)); // make a pointer type out of type i8

    FunctionType *PrintfTy = FunctionType::get(
        IntegerType::getInt32Ty(CTX), // result type = i32
        PrintfArgTy, // i8*
        true);  // isVarArg

    // add a prototype of printf and return func callee (wrapper around FunctionType and Value of the Function)
    FunctionCallee Printf = M.getOrInsertFunction("printf", PrintfTy); 
    Function *PrintfFunc = dyn_cast<Function>(Printf.getCallee()); // cast from Value to Function
    PrintfFunc->setDoesNotThrow();
    PrintfFunc->addParamAttr(0, Attribute::NoCapture);
    PrintfFunc->addParamAttr(0, Attribute::ReadOnly);


    Constant *PrintfStr = ConstantDataArray::getString(CTX, 
        "Function call: %s\n   number of arguments: %d\n");
    //Global var that holds the string format
    GlobalVariable *PrintfStrVar = dyn_cast<GlobalVariable>(M.getOrInsertGlobal("PrintfStr", PrintfStr->getType()));
    PrintfStrVar->setInitializer(PrintfStr);


    // now for each function in the module, we can instrument our printf function
    for (auto &F : M) {
        // skip declaration, we're interested only in func calls
        if (F.isDeclaration()) continue; 

        // first iterator in BB -> getting its address 
        IRBuilder<> Builder(&(*F.getEntryBlock().getFirstInsertionPt())); // passing Instruction *IP to constructor

        Constant *FuncName = Builder.CreateGlobalStringPtr(F.getName());

        // casting array [n x i8] to i8* (PrintfArgTy) required for printf
        Value *ArgStrPtr = Builder.CreatePointerCast(PrintfStrVar, PrintfArgTy);

        // inserting call to printf with arguments
        Builder.CreateCall(Printf, {ArgStrPtr, FuncName, Builder.getInt32(F.arg_size())} );

        Changed = true;
    }

    return Changed;
}

PreservedAnalyses InstrumentPrintFuncCall::run(Module &M, ModuleAnalysisManager &) {
  bool Changed =  runOnModule(M);

  return (Changed ? llvm::PreservedAnalyses::none() : llvm::PreservedAnalyses::all());
}

} //namespace instrumentprint


// -------------------------------------------------
// Pass registration as plugins in new PM interface
// -------------------------------------------------


PassPluginLibraryInfo getInstrumentPrintFuncCallPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "instrument-print-func-call", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM, ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "instrument-print-func-call") {
                    MPM.addPass(instrumentprint::InstrumentPrintFuncCall());
                    return true;
                  }
                  return false;
                });
          }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getInstrumentPrintFuncCallPluginInfo();
}