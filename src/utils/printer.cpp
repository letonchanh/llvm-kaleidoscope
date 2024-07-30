#include <iostream>

#include "printer.h"
#include "ast/NumberExprAST.h"
#include "ast/VariableExprAST.h"
#include "ast/IfExprAST.h"
#include "ast/CallExprAST.h"
#include "ast/BinaryExprAST.h"
#include "ast/PrototypeAST.h"
#include "ast/FunctionAST.h"
#include "ast/ProgramAST.h"

std::optional<Error> Printer::visit(NumberExprAST *ast)
{
    std::cout << std::string(indent, ' ') << "Num(" << ast->Val << ")" << std::endl;
    return std::nullopt;
}

std::optional<Error> Printer::visit(VariableExprAST *ast)
{
    std::cout << std::string(indent, ' ') << "Var(" << ast->Name << ")" << std::endl;
    return std::nullopt;
}

std::optional<Error> Printer::visit(CallExprAST *ast)
{
    std::cout << std::string(indent, ' ') << "Call(" << std::endl;
    indent += 2;
    std::cout << std::string(indent, ' ') << ast->Callee << std::endl;
    for (const auto &arg : ast->Args)
    {
        arg->accept(this);
    }
    indent -= 2;
    std::cout << std::string(indent, ' ') << ")" << std::endl;
    return std::nullopt;
}

std::optional<Error> Printer::visit(IfExprAST *ast)
{
    std::cout << std::string(indent, ' ') << "If(" << std::endl;
    indent += 2;
    ast->Cond->accept(this);
    indent -= 2;
    std::cout << std::string(indent, ' ') << ")" << std::endl;

    std::cout << std::string(indent, ' ') << "Then(" << std::endl;
    indent += 2;
    ast->Then->accept(this);
    indent -= 2;
    std::cout << std::string(indent, ' ') << ")" << std::endl;

    std::cout << std::string(indent, ' ') << "Else(" << std::endl;
    indent += 2;
    ast->Else->accept(this);
    indent -= 2;
    std::cout << std::string(indent, ' ') << ")" << std::endl;
    return std::nullopt;
}

std::optional<Error> Printer::visit(BinaryExprAST *ast)
{
    indent += 2;
    std::cout << std::string(indent, ' ') << ast->Op << std::endl;
    ast->LHS->accept(this);
    ast->RHS->accept(this);
    indent -= 2;
    return std::nullopt;
}

std::optional<Error> Printer::visit(PrototypeAST *ast)
{
    std::cout << std::string(indent, ' ') << "Proto(" << std::endl;
    indent += 2;
    std::cout << std::string(indent, ' ') << ast->Name << std::endl;
    for (const auto &arg : ast->Args)
    {
        std::cout << std::string(indent, ' ') << arg << std::endl;
    }
    indent -= 2;
    std::cout << std::string(indent, ' ') << ")" << std::endl;
    return std::nullopt;
}

std::optional<Error> Printer::visit(FunctionAST *ast)
{
    std::cout << std::string(indent, ' ') << "Func(" << std::endl;
    indent += 2;
    ast->Proto->accept(this);
    ast->Body->accept(this);
    indent -= 2;
    std::cout << std::string(indent, ' ') << ")" << std::endl;
    return std::nullopt;
}

std::optional<Error> Printer::visit(ProgramAST *ast)
{
    for (const auto &node : ast->Nodes)
    {
        node->accept(this);
    }
    return std::nullopt;
}