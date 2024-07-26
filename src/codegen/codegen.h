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

#include "visitor/visitor.h"

class CodeGen : public Visitor
{
    // This is an object that owns LLVM core data structures
    std::unique_ptr<llvm::LLVMContext> Context;

    // This is a helper object that makes easy to generate LLVM instructions
    std::unique_ptr<llvm::IRBuilder<>> Builder;

    // This map keeps track of which values are defined in the current scope
    std::map<std::string, llvm::Value *> NamedValues;

    // exprVal holds the intermediate value when visiting an expression.
    // The destructor of llvm::Value is protected, so that it cannot be called
    // by the destructor of unique_ptr. We need to provide a custom destructor,
    // which does nothing.
    std::unique_ptr<llvm::Value, std::function<void *(llvm::Value *)>> exprVal;

public:
    CodeGen();

    // This is an LLVM construct that contains functions and global variables
    std::unique_ptr<llvm::Module> Module;

    std::optional<Error> visit(NumberExprAST *ast) override;
    std::optional<Error> visit(VariableExprAST *ast) override;
    std::optional<Error> visit(CallExprAST *ast) override;
    std::optional<Error> visit(BinaryExprAST *ast) override;
    std::optional<Error> visit(PrototypeAST *ast) override;
    std::optional<Error> visit(FunctionAST *ast) override;
    std::optional<Error> visit(ProgramAST *ast) override;
};

#endif