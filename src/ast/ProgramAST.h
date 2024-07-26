#ifndef __PROGRAM_AST_H__
#define __PROGRAM_AST_H__

#include "AST.h"

class ProgramAST : public AST
{
public:
    ProgramAST(std::vector<std::unique_ptr<AST>> nodes)
        : Nodes(std::move(nodes)) {}

    std::optional<Error> accept(Visitor *visitor) override
    {
        return visitor->visit(this);
    }

    std::vector<std::unique_ptr<AST>> Nodes;
};

#endif