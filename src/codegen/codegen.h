#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <map>
#include <stack>
#include <memory>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"

#include "visitor/visitor.h"
#include "optimizer.h"
#include "jit/jit_manager.h"
#include "utils/result.h"
#include "utils/error.h"

class CodeGen : public Visitor
{
    // This is a helper object that makes easy to generate LLVM instructions
    std::unique_ptr<llvm::IRBuilder<>> Builder;

    // This map keeps track of which values are defined in the current scope
    std::map<std::string, llvm::Value *> NamedValues;

    // exprVal holds the intermediate value when visiting an expression.
    // The destructor of llvm::Value is protected, so that it cannot be called
    // by the destructor of unique_ptr. We need to provide a custom destructor,
    // which does nothing.
    std::unique_ptr<llvm::Value, std::function<void *(llvm::Value *)>> exprVal;

    std::unique_ptr<Optimizer> optimizer;

    std::unique_ptr<JITManager> jm;

    std::map<std::string, PrototypeAST *> fnProtos;

    void init();

    Result<llvm::Function *, Error> getFunction(std::string fnName);

    void deleteFn(llvm::Function *fn, bool isDeclared);

public:
    CodeGen();

    // ~CodeGen()
    // {
    //     jm->JIT->getMainJITDylib().clear();
    // }

    // This is an LLVM construct that contains functions and global variables
    std::unique_ptr<llvm::Module> Module;

    // This is an object that owns LLVM core data structures
    std::unique_ptr<llvm::LLVMContext> Context;

    std::optional<Error> visit(NumberExprAST *ast) override;
    std::optional<Error> visit(VariableExprAST *ast) override;
    std::optional<Error> visit(CallExprAST *ast) override;
    std::optional<Error> visit(IfExprAST *ast) override;
    std::optional<Error> visit(BinaryExprAST *ast) override;
    std::optional<Error> visit(PrototypeAST *ast) override;
    std::optional<Error> visit(FunctionAST *ast) override;
    std::optional<Error> visit(ProgramAST *ast) override;
};

#endif