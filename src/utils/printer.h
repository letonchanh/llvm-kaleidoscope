#ifndef __PRINTER_H__
#define __PRINTER_H__

#include "visitor/visitor.h"

class Printer : public Visitor
{
    int indent = 0;

public:
    std::optional<Error> visit(NumberExprAST *ast) override;
    std::optional<Error> visit(VariableExprAST *ast) override;
    std::optional<Error> visit(CallExprAST *ast) override;
    std::optional<Error> visit(IfExprAST *ast) override;
    std::optional<Error> visit(BinaryExprAST *ast) override;
    std::optional<Error> visit(PrototypeAST *ast) override;
    std::optional<Error> visit(FunctionAST *ast) override;
    std::optional<Error> visit(ProgramAST *ast) override;
};

#endif