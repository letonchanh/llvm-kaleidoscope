#include <iostream>

#include "codegen.h"

#include "logger/logger.h"

#include "ast/BinaryExprAST.h"
#include "ast/CallExprAST.h"
#include "ast/FunctionAST.h"
#include "ast/NumberExprAST.h"
#include "ast/ProgramAST.h"
#include "ast/PrototypeAST.h"
#include "ast/VariableExprAST.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Verifier.h"

CodeGen::CodeGen()
{
    Context = std::make_unique<llvm::LLVMContext>();
    Module = std::make_unique<llvm::Module>("kaleidoscope", *Context);
    Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
}

std::optional<Error> CodeGen::visit(NumberExprAST *ast)
{
    auto v = llvm::ConstantFP::get(*Context, llvm::APFloat(ast->Val));
    exprVal.reset(v);
    return std::nullopt;
}

std::optional<Error> CodeGen::visit(VariableExprAST *ast)
{
    auto v = NamedValues[ast->Name];
    if (!v)
    {
        return Error("unknown variable name");
    }
    exprVal.reset(v);
    return std::nullopt;
}

std::optional<Error> CodeGen::visit(CallExprAST *ast)
{
    // Look up the name in the global module table.
    llvm::Function *calleeFn = Module->getFunction(ast->Callee);
    if (!calleeFn)
    {
        return Error("unknown function referenced");
    }

    // If argument mismatch error.
    if (calleeFn->arg_size() != ast->Args.size())
    {
        return Error("incorrect # arguments passed");
    }

    std::vector<llvm::Value *> argsV;
    for (const auto &arg : ast->Args)
    {
        if (auto err = arg->accept(this))
        {
            return err;
        }
        argsV.push_back(exprVal.release());
    }

    llvm::Value *result = Builder->CreateCall(calleeFn, argsV, "calltmp");
    exprVal.reset(result);
    return std::nullopt;
}

std::optional<Error> CodeGen::visit(BinaryExprAST *ast)
{
    if (auto err = ast->LHS->accept(this))
        return err;
    llvm::Value *lval = exprVal.release();

    if (auto err = ast->RHS->accept(this))
        return err;
    llvm::Value *rval = exprVal.release();

    llvm::Value *result = nullptr;
    switch (ast->Op)
    {
    case '+':
        result = Builder->CreateFAdd(lval, rval, "addtmp");
        break;
    case '-':
        result = Builder->CreateFSub(lval, rval, "subtmp");
        break;
    case '*':
        result = Builder->CreateFMul(lval, rval, "multmp");
        break;
    case '<':
        result = Builder->CreateFCmpULT(lval, rval, "cmptmp");
        // Convert bool 0 or 1 to double 0.0 or 1.0
        result = Builder->CreateUIToFP(
            result,
            llvm::Type::getDoubleTy(*Context),
            "booltmp");
        break;
    default:
        return Error("invalid binary operator");
    }
    exprVal.reset(result);
    return std::nullopt;
}

std::optional<Error> CodeGen::visit(PrototypeAST *ast)
{
    // First, check for an existing function from a previous 'extern' declaration.
    llvm::Function *fn = Module->getFunction(ast->Name);
    if (fn)
    {
        if (!fn->empty())
        {
            return Error("function redefined");
        }
        if (fn->arg_size() != ast->Args.size())
        {
            return Error("# params mismatched");
        }
    }

    if (!fn)
    {
        // Make the function type: e.g double(double,double) etc.
        std::vector<llvm::Type *> Doubles(ast->Args.size(), llvm::Type::getDoubleTy(*Context));
        llvm::FunctionType *fnType =
            llvm::FunctionType::get(llvm::Type::getDoubleTy(*Context), Doubles, false);

        // The created function would be added into Module.
        fn = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, ast->Name, *Module);

        // Print the newly created function
        fn->print(llvm::errs());
    }

    // Set names for all arguments.
    unsigned idx = 0;
    for (auto &arg : fn->args())
        arg.setName(ast->Args[idx++]);

    return std::nullopt;
}

void deleteFn(llvm::Function *fn, bool isDeclared)
{
    if (isDeclared)
        fn->deleteBody();
    else
        fn->removeFromParent();
}

std::optional<Error> CodeGen::visit(FunctionAST *ast)
{
    bool isDeclared = false;
    if (auto fn = Module->getFunction(ast->Proto->Name))
    {
        isDeclared = true;
    }
    if (auto err = ast->Proto->accept(this))
        return err;

    llvm::Function *fn = Module->getFunction(ast->Proto->Name);

    // Create a new basic block to start insertion into.
    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*Context, "entry", fn);
    Builder->SetInsertPoint(bb);

    // Record the function arguments in the NamedValues map.
    NamedValues.clear();
    for (auto &arg : fn->args())
        NamedValues[std::string(arg.getName())] = &arg;

    // Created instructions from Body should be appended
    // to the end of the basic block bb.
    if (auto err = ast->Body->accept(this))
    {
        deleteFn(fn, isDeclared);
        return err;
    }
    llvm::Value *retVal = exprVal.release();

    // Error reading body, remove function.
    if (!retVal)
    {
        deleteFn(fn, isDeclared);
        return Error("failed to generate body's code");
    }

    // Finish off the function.
    Builder->CreateRet(retVal);

    // Validate the generated code, checking for consistency.
    bool hasErrors = llvm::verifyFunction(*fn, &llvm::errs());
    if (hasErrors)
    {
        deleteFn(fn, isDeclared);
        return Error("failed to verify function");
    }

    // Print the newly created function
    fn->print(llvm::errs());

    return std::nullopt;
}

std::optional<Error> CodeGen::visit(ProgramAST *ast)
{
    for (const auto &node : ast->Nodes)
    {
        if (auto err = node->accept(this))
            return err;
    }
    return std::nullopt;
}