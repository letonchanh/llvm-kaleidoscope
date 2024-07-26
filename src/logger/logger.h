#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <memory>

#include "ast/ExprAST.h"
#include "ast/PrototypeAST.h"
#include "ast/FunctionAST.h"

#include "llvm/IR/Value.h"

std::unique_ptr<ExprAST> ParseError(const char *Str);

std::unique_ptr<PrototypeAST> ParseErrorProto(const char *Str);

std::unique_ptr<FunctionAST> ParseErrorFunc(const char *Str);

void CodegenError(const char *Str);

#endif