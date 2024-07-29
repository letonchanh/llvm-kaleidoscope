#ifndef __OPTIMIZER_H__
#define __OPTIMIZER_H__

#include <memory>

#include "llvm/IR/PassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Analysis/LoopAnalysisManager.h"

class Optimizer
{
    // Passes and analysis managers.
    std::unique_ptr<llvm::FunctionPassManager> fpm;
    std::unique_ptr<llvm::LoopAnalysisManager> lam;
    std::unique_ptr<llvm::FunctionAnalysisManager> fam;
    std::unique_ptr<llvm::CGSCCAnalysisManager> cgam;
    std::unique_ptr<llvm::ModuleAnalysisManager> mam;

    std::unique_ptr<llvm::PassInstrumentationCallbacks> pic;
    std::unique_ptr<llvm::StandardInstrumentations> si;

public:
    Optimizer(llvm::LLVMContext &context);

    void optimizeFn(llvm::Function &fn);
};

#endif