// ====================================================
// FILE:
//     StaticCallCounterl.h
//
// DESCRIPTION:
//     
//        
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#ifndef STATICCALLCOUNTER_H
#define STATICCALLCOUNTER_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/IR/Module.h"


namespace staticcallc {

struct StaticCallCounter : public llvm::AnalysisInfoMixin<StaticCallCounter> {
    using Result = llvm::MapVector<const llvm::Function *, unsigned>;

    Result run(llvm::Module &M, llvm::ModuleAnalysisManager &);

    static bool isRequired() { return true; }

private:
    static llvm::AnalysisKey Key;
    friend struct llvm::AnalysisInfoMixin<StaticCallCounter>;
};


// Printer Pass for the analysis pass
class StaticCallCounterPrinter : public llvm::PassInfoMixin<StaticCallCounterPrinter> {
public:
  explicit StaticCallCounterPrinter(llvm::raw_ostream &OS) : OS(OS) {}
  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM);

  static bool isRequired() { return true; }

private:
  llvm::raw_ostream &OS;
};



} // namespace staticcallc


#endif // STATICCALLCOUNTER_H