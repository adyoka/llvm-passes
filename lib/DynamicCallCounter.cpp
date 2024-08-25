// ====================================================
// FILE:
//     DynamicCallCounter.cpp
//
// DESCRIPTION:
//     A pass that counts the dynamic function calls.
//     In ordere to count the dynamic, run-time function calls, one has to instrument the module for it to print the counter while executing.
//     
//     Implementation:
//     For every function F in the input module M, we instrument and store a i32 variable (counter) that is incremented
//     everytime F is called, and store the pointer to the variable in a map.
//     At the end of the module, we isntrumented and call a function wrapper that prints the results of the counter map
//
// DEVELOPED BY:
//     Adilet Majit 2024
//     Following llvm-tutor
// ====================================================

#include "DynamicCallCounter.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

#include "llvm/Transforms/Utils/ModuleUtils.h" // for appendToGlobalDtors()

using namespace llvm;

namespace dynamicallc {

bool DynamicCallCounter::runOnModule(Module &M) {
    bool Changed = false;

    StringMap<Constant *> CallCountMap;
    StringMap<Constant *> FuncNameMap;
    
    auto &CTX = M.getContext();

    // traverse each function in the module and instrument call counter
    for (auto &F : M) {
        if (F.isDeclaration()) continue;

        // builder for instrumentation of code at the beginning of each function
        IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());

        // variable for the counter of function calls
        std::string counterVarName = "counter_" + std::string(F.getName());
        Constant *GlobalVar = M.getOrInsertGlobal(counterVarName, IntegerType::getInt32Ty(CTX));

        // get the variable by var name, align and init to unsigned int 0
        GlobalVariable *GV = M.getNamedGlobal(counterVarName);
        GV->setLinkage(GlobalValue::CommonLinkage);
        GV->setAlignment(MaybeAlign(4));
        GV->setInitializer(ConstantInt::get(CTX, APInt(32, 0)));

        CallCountMap[F.getName()] = GlobalVar;

        auto FuncName = Builder.CreateGlobalStringPtr(F.getName());
        FuncNameMap[F.getName()] = FuncName;

        // increment by 1 the counter and store
        LoadInst *Load1 = Builder.CreateLoad(IntegerType::getInt32Ty(CTX), GlobalVar);
        Value *IncrementRes = Builder.CreateAdd(Load1, Builder.getInt32(1));
        Builder.CreateStore(IncrementRes, GlobalVar);

        Changed = true;
    }

    if (Changed == false) return Changed;


    // Same as in InstrumentFuncCall pass
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

    Constant *ResultStr = ConstantDataArray::getString(CTX, "%-20s %-10lu\n");
    Constant *ResultStrVar = M.getOrInsertGlobal("ResultStrIRVar", ResultStr->getType());
    dyn_cast<GlobalVariable>(ResultStrVar)->setInitializer(ResultStr);


    std::string str = "=================================================\n";
    str += "Dynamic Call Counter analysis results: \n";
    str += "=================================================\n";
    str += "NAME                 # OF DIRECT DYNAMIC CALLS\n";
    str += "-------------------------------------------------\n";

    Constant *ResultHeaderStr = ConstantDataArray::getString(CTX, str.c_str());
    Constant *ResultHeaderStrVar = M.getOrInsertGlobal("ResultHeaderIRVar", ResultHeaderStr->getType());
    dyn_cast<GlobalVariable>(ResultHeaderStrVar)->setInitializer(ResultHeaderStr);

    // ---------------------------------------------
    // create a function wrapper that will print the results of the cotuner map
    FunctionType *PrintfWrapperTy = FunctionType::get(Type::getVoidTy(CTX), {}, false);
    Function* PrintfWrapperFunc = dyn_cast<Function>(
      M.getOrInsertFunction("printf_wrapper", PrintfWrapperTy).getCallee());

    // create basic block and builder to instrument IR code
    BasicBlock *BB = BasicBlock::Create(CTX, "wrapper_entry", PrintfWrapperFunc);
    IRBuilder<> Builder(BB);

    // cast char i8 array type to i8* pointer type
    Value *ResultHeaderStrPtr = Builder.CreatePointerCast(ResultHeaderStrVar, PrintfArgTy);
    Value *ResultCounterStrPtr = Builder.CreatePointerCast(ResultStrVar, PrintfArgTy);

    // instrument a call to printf function for printing header string
    Builder.CreateCall(Printf, {ResultHeaderStrPtr});

    LoadInst *LoadCount;
    for (auto &iter : CallCountMap) {
      LoadCount = Builder.CreateLoad(IntegerType::getInt32Ty(CTX), iter.second);
      Builder.CreateCall(Printf, { ResultCounterStrPtr, FuncNameMap[iter.first()], LoadCount } );
    }
    // return from wrapper function
    Builder.CreateRetVoid();
    // ------- end of function wrapper -----------

    // Call PrintfWrapperFunc at the end (lowest 0 priority) of this module M
    appendToGlobalDtors(M, PrintfWrapperFunc, 0);


    return Changed;
}

PreservedAnalyses DynamicCallCounter::run(Module &M, ModuleAnalysisManager &) {
  bool Changed = runOnModule(M);

  return (Changed ? PreservedAnalyses::none() : PreservedAnalyses::all());
}


} // namespace dynamicallc



// -------------------------------------------------
// Pass registration as plugins in new PM interface
// -------------------------------------------------


llvm::PassPluginLibraryInfo getDynamicCallCounterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "static-cc", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, ModulePassManager &MPM, ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "dynamic-cc") {
                    MPM.addPass(dynamicallc::DynamicCallCounter());
                    return true;
                  }
                  return false;
                });
          }};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getDynamicCallCounterPluginInfo();
}