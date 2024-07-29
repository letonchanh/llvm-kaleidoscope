#ifndef __JIT_MANAGER_H__
#define __JIT_MANAGER_H__

#include <memory>

#include "jit.h"
#include "llvm/Support/Error.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"

class JITManager
{

public:
    JITManager();
    void JITExec(std::unique_ptr<llvm::Module> module, std::unique_ptr<llvm::LLVMContext> context);
    void JITAddModule(std::unique_ptr<llvm::Module> module, std::unique_ptr<llvm::LLVMContext> context);

    std::unique_ptr<llvm::orc::KaleidoscopeJIT> JIT;
    llvm::ExitOnError ExitOnErr;
};

#endif