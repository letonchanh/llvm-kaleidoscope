#ifndef __AST_H__
#define __AST_H__

#include "visitor/visitor.h"

class AST
{
public:
    virtual ~AST() = default;
    virtual std::optional<Error> accept(Visitor *visitor) = 0;
};

#endif