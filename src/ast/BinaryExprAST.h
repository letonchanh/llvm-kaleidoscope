#ifndef __BINARY_EXPR_AST_H__
#define __BINARY_EXPR_AST_H__

#include <memory>
#include "ExprAST.h"

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST
{
public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
        : Op(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {}

    std::optional<Error> accept(Visitor *visitor) override
    {
        return visitor->visit(this);
    }

    char Op;
    std::unique_ptr<ExprAST> LHS, RHS;
};

#endif