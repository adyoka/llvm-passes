// ====================================================
// FILE:
//     DCEMain.cpp
//
// DESCRIPTION:
//     This command line tool removes all the (obviously) dead instructions,
//     and then removes any newly dead code by rechecking instructions used by removed ones.
//      
//     
// DEVELOPED BY:
//     Adilet Majit 2024
// ====================================================

#include "DeadCodeElimination.h"

#include "llvm/IRReader/IRReader.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static cl::OptionCategory DCECategory("dead code elimination tool options");

static cl::opt<std::string> InputModule(cl::Positional,
                                       cl::desc("<Input module>"),
                                       cl::value_desc("bitcode filename"),
                                       cl::init(""),
                                       cl::Required,
                                       cl::cat(DCECategory));

static cl::opt<std::string> OutputModule("o", cl::Positional,
                                         cl::desc("<Output module>"),
                                         cl::value_desc("bitcode filename"),
                                         cl::init("out.ll"),
                                         cl::cat(DCECategory));


static void eliminateDeadCode(Module &M) {
    DeadCodeElimPass Pass;
    FunctionPassManager FPM;
    FPM.addPass(Pass);

    FunctionAnalysisManager FAM;
    PassBuilder PB;
    PB.registerFunctionAnalyses(FAM);

    for (Function &F : M) 
        FPM.run(F, FAM);

}

int main(int argc, char **argv) {

    cl::HideUnrelatedOptions(DCECategory);

    cl::ParseCommandLineOptions(argc, argv, "Eliminate dead code in the input IR file.\n");

    // Makes sure llvm_shutdown() is called (which cleans up LLVM objects)
    //  http://llvm.org/docs/ProgrammersManual.html#ending-execution-with-llvm-shutdown
    llvm_shutdown_obj SDO;

    SMDiagnostic Err;
    LLVMContext CTX;
    std::unique_ptr<Module> M = parseIRFile(InputModule.getValue(), Err, CTX);

    if (!M) {
        errs() << "Error reading bitcode file: " << InputModule << "\n";
        Err.print(argv[0], errs());
        return -1;
    }

    eliminateDeadCode(*M);

    std::error_code EC;
    raw_fd_ostream OS(OutputModule.getValue(), EC);

    if (EC) {
        errs() << "Couldn't open " << OutputModule.getValue() << ": " << EC.message() << "\n";
        return -1;
    }

    // Write the transformed code into the output file.
    M->print(OS, nullptr);
    OS.close();

    return 0;
}
