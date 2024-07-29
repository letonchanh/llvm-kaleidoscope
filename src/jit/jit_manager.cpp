#include "jit_manager.h"

#include <cstdio>
#include "llvm/Support/TargetSelect.h"

JITManager::JITManager()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    JIT = ExitOnErr(llvm::orc::KaleidoscopeJIT::Create());
}

void JITManager::JITAddModule(std::unique_ptr<llvm::Module> module, std::unique_ptr<llvm::LLVMContext> context)
{
    ExitOnErr(JIT->addModule(
        llvm::orc::ThreadSafeModule(std::move(module), std::move(context))));
}

void JITManager::JITExec(std::unique_ptr<llvm::Module> module, std::unique_ptr<llvm::LLVMContext> context)
{
    auto rt = JIT->getMainJITDylib().createResourceTracker();

    auto tsm = llvm::orc::ThreadSafeModule(std::move(module), std::move(context));
    ExitOnErr(JIT->addModule(std::move(tsm), rt));

    // Search the JIT for the __main__ symbol.
    auto ExprSymbol = ExitOnErr(JIT->lookup("__main__"));

    // Get the symbol's address and cast it to the right type (takes no
    // arguments, returns a double) so we can call it as a native function.
    double (*fp)() = ExprSymbol.getAddress().toPtr<double (*)()>();
    fprintf(stderr, "*** Evaluated to %f\n", fp());

    // Delete the anonymous expression module from the JIT.
    ExitOnErr(rt->remove());
}