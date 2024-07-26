#ifndef __NUMBER_EXPR_AST_H__
#define __NUMBER_EXPR_AST_H__

#include "ExprAST.h"

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST
{

public:
    NumberExprAST(double val) : Val(val) {}

    std::optional<Error> accept(Visitor *visitor) override
    {
        return visitor->visit(this);
    }

    double Val;
};

#endif