// ====================================================
// FILE:
//     OpcodeCounter.h
//
// DESCRIPTION:
//     Header file that declares an OpcodeCounter Pass 
//          (new Pass Manager interface and printer pass for the analysis pass)
//
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================


#ifndef OPCODECOUNTER_H
#define OPCODECOUNTER_H


#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Function.h"

namespace opcodecounter {

// Analysis pass using new PM interface
struct OpcodeCounter : public llvm::AnalysisInfoMixin<OpcodeCounter> {
    using Result = llvm::StringMap<unsigned>;
    Result run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM);

    static bool isRequired() { return true; }

private:
    static llvm::AnalysisKey Key;
    friend struct llvm::AnalysisInfoMixin<OpcodeCounter>;

};

// Printer pass for our analysis pass
class OpcodeCounterPrinter : public llvm::PassInfoMixin<OpcodeCounterPrinter> {
public:
  explicit OpcodeCounterPrinter(llvm::raw_ostream &OutS) : OS(OutS) {}
  llvm::PreservedAnalyses run(llvm::Function &Func, llvm::FunctionAnalysisManager &FAM);

  static bool isRequired() { return true; }

private:
  llvm::raw_ostream &OS;
}; 

} // namespace opcodecounter

#endif // OPCODECOUNTER_H
