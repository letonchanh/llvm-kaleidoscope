#ifndef __VISITOR_H__
#define __VISITOR_H__

#include <optional>
#include "error/error.h"

class NumberExprAST;
class VariableExprAST;
class CallExprAST;
class BinaryExprAST;
class PrototypeAST;
class FunctionAST;
class ProgramAST;

class Visitor
{
public:
    virtual ~Visitor() = default;

    virtual std::optional<Error> visit(NumberExprAST *ast) = 0;
    virtual std::optional<Error>visit(VariableExprAST *ast) = 0;
    virtual std::optional<Error>visit(CallExprAST *ast) = 0;
    virtual std::optional<Error>visit(BinaryExprAST *ast) = 0;

    virtual std::optional<Error>visit(PrototypeAST *ast) = 0;
    virtual std::optional<Error>visit(FunctionAST *ast) = 0;

    virtual std::optional<Error>visit(ProgramAST *ast) = 0;
};

#endif
