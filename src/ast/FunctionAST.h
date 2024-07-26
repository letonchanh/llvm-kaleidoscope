#ifndef __FUNCTION_AST_H__
#define __FUNCTION_AST_H__

#include "AST.h"
#include "PrototypeAST.h"
#include "ExprAST.h"

class FunctionAST : public AST
{
public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> body)
        : Proto(std::move(proto)), Body(std::move(body)) {}

    std::optional<Error> accept(Visitor *visitor) override
    {
        return visitor->visit(this);
    }

    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;
};

#endif