#ifndef __PROTOTYPE_AST_H__
#define __PROTOTYPE_AST_H__

#include <string>
#include <vector>

#include "AST.h"

class PrototypeAST : public AST
{
public:
  PrototypeAST(const std::string &name, std::vector<std::string> args)
      : Name(name), Args(std::move(args)) {}

  const std::string &getName() const { return Name; }

  std::optional<Error> accept(Visitor *visitor) override
  {
    return visitor->visit(this);
  }

  std::string Name;
  std::vector<std::string> Args;
};

#endif