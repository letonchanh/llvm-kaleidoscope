#include "optimizer.h"

#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"

#include "llvm/Passes/PassBuilder.h"

Optimizer::Optimizer(llvm::LLVMContext &context)
{
    // Create new nalysis managers.
    // Module -> (CGSCC ->) Function -> Loop
    mam = std::make_unique<llvm::ModuleAnalysisManager>();
    cgam = std::make_unique<llvm::CGSCCAnalysisManager>();
    fam = std::make_unique<llvm::FunctionAnalysisManager>();
    lam = std::make_unique<llvm::LoopAnalysisManager>();

    pic = std::make_unique<llvm::PassInstrumentationCallbacks>();
    si = std::make_unique<llvm::StandardInstrumentations>(context,
                                                          /*DebugLogging*/ true);
    si->registerCallbacks(*pic, mam.get());

    // Create new function pass manager and add function-level passes to it.
    fpm = std::make_unique<llvm::FunctionPassManager>();
    // Add transform passes.
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    fpm->addPass(llvm::InstCombinePass());
    // Reassociate expressions.
    fpm->addPass(llvm::ReassociatePass());
    // Eliminate Common SubExpressions.
    fpm->addPass(llvm::GVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    fpm->addPass(llvm::SimplifyCFGPass());

    // Register analysis passes used in these transform passes.
    llvm::PassBuilder pb;
    pb.registerModuleAnalyses(*mam);
    pb.registerFunctionAnalyses(*fam);
    pb.crossRegisterProxies(*lam, *fam, *cgam, *mam);
}

void Optimizer::optimizeFn(llvm::Function &fn)
{
    fpm->run(fn, *fam);
}
