#ifndef __IF_EXPR_AST__
#define __IF_EXPR_AST__

#include <memory>
#include "ExprAST.h"

class IfExprAST : public ExprAST
{
public:
    IfExprAST(
        std::unique_ptr<ExprAST> _cond,
        std::unique_ptr<ExprAST> _then,
        std::unique_ptr<ExprAST> _else)
        : Cond(std::move(_cond)), Then(std::move(_then)), Else(std::move(_else)) {}

    std::optional<Error> accept(Visitor *visitor) override
    {
        return visitor->visit(this);
    }

    std::unique_ptr<ExprAST> Cond, Then, Else;
};

#endif