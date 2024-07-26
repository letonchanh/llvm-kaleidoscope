#ifndef __VARIABLE_EXPR_AST_H__
#define __VARIABLE_EXPR_AST_H__

#include <string>
#include "ExprAST.h"

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST
{
public:
    VariableExprAST(const std::string &name) : Name(name) {}

    std::optional<Error> accept(Visitor *visitor) override
    {
        return visitor->visit(this);
    }

    std::string Name;
};

#endif