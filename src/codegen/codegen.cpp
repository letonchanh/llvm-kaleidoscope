#include <iostream>

#include "codegen.h"

#include "ast/BinaryExprAST.h"
#include "ast/CallExprAST.h"
#include "ast/FunctionAST.h"
#include "ast/NumberExprAST.h"
#include "ast/ProgramAST.h"
#include "ast/PrototypeAST.h"
#include "ast/VariableExprAST.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Verifier.h"

CodeGen::CodeGen()
{
    // We only use a single JIT to manage multiple modules.
    jm = std::make_unique<JITManager>();
    init();
}

void CodeGen::init()
{
    // Open a new context and module.
    Context = std::make_unique<llvm::LLVMContext>();
    Module = std::make_unique<llvm::Module>("kaleidoscope", *Context);
    // When a module is created, set JIT's data layout to it
    // so that the generated code in this module is consistent with JIT.
    Module->setDataLayout(jm->JIT->getDataLayout());

    // Create a new builder for the module.
    Builder = std::make_unique<llvm::IRBuilder<>>(*Context);

    // Create a new optimizer
    optimizer = std::make_unique<Optimizer>(*Context);
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
    auto r = getFunction(ast->Callee);
    if (r.isError())
        return r.error();
    llvm::Function *calleeFn = r.value();
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

Result<llvm::Function *, Error> CodeGen::getFunction(std::string fnName)
{
    if (auto *fn = Module->getFunction(fnName))
    {
        return Result<llvm::Function *, Error>(fn);
    }
    if (fnProtos.count(fnName) > 0)
    {
        // Visit the function's PrototypeAST to add its declaration
        // to the current module in its first encounter.
        if (auto err = fnProtos[fnName]->accept(this))
        {
            return Result<llvm::Function *, Error>(err.value());
        }
    }
    return Result<llvm::Function *, Error>(Module->getFunction(fnName));
}

std::optional<Error> CodeGen::visit(PrototypeAST *ast)
{
    // First, check for an existing function from a previous 'extern' declaration.
    llvm::Function *fn = Module->getFunction(ast->Name);
    if (fn)
    {
        if (!fn->empty())
        {
            return Error("redeclare a defined function");
        }
        if (fn->arg_size() != ast->Args.size())
        {
            return Error("# params mismatched");
        }
    }
    else if (fnProtos.count(ast->Name) > 0)
    {
        // This prototype has been defined in other module before,
        // check if the prototype is consistent with the previous definition.
        // JIT will return an error if two duplicate definitions are added.
        if (ast->Args.size() != fnProtos[ast->Name]->Args.size())
            return Error("# params mismatched");
    }

    if (!fn)
    {
        // Make the function type: e.g double(double,double) etc.
        std::vector<llvm::Type *> Doubles(ast->Args.size(), llvm::Type::getDoubleTy(*Context));
        llvm::FunctionType *fnType =
            llvm::FunctionType::get(llvm::Type::getDoubleTy(*Context), Doubles, false);

        // The created function would be added into Module.
        fn = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, ast->Name, *Module);
    }

    // Set names for all arguments.
    unsigned idx = 0;
    for (auto &arg : fn->args())
        arg.setName(ast->Args[idx++]);

    fnProtos[ast->Name] = ast;

    return std::nullopt;
}

void CodeGen::deleteFn(llvm::Function *fn, bool isDeclared)
{
    if (isDeclared)
    {
        fn->deleteBody();
    }
    else
    {
        fnProtos.erase(fn->getName().str());
        fn->removeFromParent();
    }
}

std::optional<Error> CodeGen::visit(FunctionAST *ast)
{
    bool isDeclared = false;
    auto r = getFunction(ast->Proto->Name);
    if (r.isError())
        return r.error();
    if (auto fn = r.value())
    {
        isDeclared = true;
    }

    if (auto err = ast->Proto->accept(this))
        return err;

    r = getFunction(ast->Proto->Name);
    if (r.isError())
        return r.error();
    llvm::Function *fn = r.value();

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
    fprintf(stderr, "*** Generated function:\n");
    fn->print(llvm::errs());

    optimizer->optimizeFn(*fn);

    // Print the optimized function
    fprintf(stderr, "*** Optimized function:\n");
    fn->print(llvm::errs());

    if (ast->Proto->Name == "__main__")
    {
        bool broken = llvm::verifyModule(*Module, &llvm::errs());
        if (broken)
        {
            fprintf(stderr, "Generated IR broken\n");
            return std::nullopt;
        }

        fprintf(stderr, "*** Main module:\n");
        Module->print(llvm::errs(), nullptr);

        // Move the current module and context containing __main__
        // into JIT to execute.
        jm->JITExec(std::move(Module), std::move(Context));
    }
    else
    {
        // Move the current module and context, which contain
        // the function's implementation, to JIT.
        jm->JITAddModule(std::move(Module), std::move(Context));
    }
    // Create new module and context for the next function.
    init();

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