#include "logger.h"

std::unique_ptr<ExprAST> ParseError(const char *Str)
{
    fprintf(stderr, "ParseError: %s\n", Str);
    return nullptr;
}

std::unique_ptr<PrototypeAST> ParseErrorProto(const char *Str)
{
    ParseError(Str);
    return nullptr;
}

std::unique_ptr<FunctionAST> ParseErrorFunc(const char *Str)
{
    ParseError(Str);
    return nullptr;
}

void CodegenError(const char *Str) {
    fprintf(stderr, "CodegenError: %s\n", Str);
}
