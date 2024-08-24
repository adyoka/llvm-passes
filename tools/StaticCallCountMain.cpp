// ====================================================
// FILE:
//     StaticCallCountMain.cpp
//
// DESCRIPTION:
//     This command line tool counts all static function calls in the input IR file.
//     See implementation of the underlying pass in "../StaticCallCounter.cpp" file.
//      
//     
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#include "StaticCallCounter.h"

#include "llvm/IRReader/IRReader.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static cl::OptionCategory CallCounterCategory{"Static call counter options"};

static cl::opt<std::string> InputModule(cl::Positional,
                                       cl::desc("<Input module>"),
                                       cl::value_desc("bitcode filename"),
                                       cl::init(""),
                                       cl::Required,
                                       cl::cat(CallCounterCategory));



int main(int argc, char **argv) {
  // Hide all options apart from the ones specific to this tool
  cl::HideUnrelatedOptions(CallCounterCategory);

  cl::ParseCommandLineOptions(argc, argv, "Counts the number of static function calls\n");

  // Makes sure llvm_shutdown() is called (which cleans up LLVM objects)
  //  http://llvm.org/docs/ProgrammersManual.html#ending-execution-with-llvm-shutdown
  llvm_shutdown_obj SDO;

  // Parse the IR file passed on the command line
  SMDiagnostic Err;
  LLVMContext Ctx;
  std::unique_ptr<Module> M = parseIRFile(InputModule.getValue(), Err, Ctx);

  if (!M) {
    errs() << "Error reading bitcode file: " << InputModule << "\n";
    Err.print(argv[0], errs());
    return -1;
  }

  ModulePassManager MPM;
  MPM.addPass(staticcallc::StaticCallCounterPrinter(llvm::errs()));

  
  ModuleAnalysisManager MAM;
  MAM.registerPass([&] { return staticcallc::StaticCallCounter(); });

  PassBuilder PB;
  PB.registerModuleAnalyses(MAM);

  MPM.run(*M, MAM);

  return 0;
}
