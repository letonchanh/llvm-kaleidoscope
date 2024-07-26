#ifndef __CALL_EXPR_AST_H__
#define __CALL_EXPR_AST_H__

#include <string>
#include <memory>
#include <utility>
#include "ExprAST.h"

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST
{
public:
    CallExprAST(const std::string &callee, std::vector<std::unique_ptr<ExprAST>> args)
        : Callee(callee), Args(std::move(args)) {}

    std::optional<Error> accept(Visitor *visitor) override
    {
        return visitor->visit(this);
    }

    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;
};

#endif