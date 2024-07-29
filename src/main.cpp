#include <memory>
#include <iostream>
#include <cstdlib>

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/ManagedStatic.h"

#include "lexer/input.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser/parser.h"
#include "utils/printer.h"
#include "codegen/codegen.h"

using namespace std;

class StdIn : public IInput
{
public:
    StdIn() = default;

    int GetChar() override { return getchar(); }
};

int main()
{
    std::unique_ptr<IInput> stdInput = std::make_unique<StdIn>();
    std::unique_ptr<Lexer> lexer = std::make_unique<Lexer>(std::move(stdInput));
    fprintf(stderr, "ready> ");
    lexer->GetNextToken();
    std::unique_ptr<Parser> parser = std::make_unique<Parser>(std::move(lexer));

    std::unique_ptr<Printer> printer = std::make_unique<Printer>();
    std::unique_ptr<CodeGen> codegen = std::make_unique<CodeGen>();
    std::vector<Visitor *> visitors;
    visitors.push_back(printer.get());
    visitors.push_back(codegen.get());

    ParseResult r = parser->ParseProgram(visitors);
    if (r.isError())
    {
        fprintf(stderr, "%s", r.error().message.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

    // ast->accept(printer.get());
    // if (auto err = ast->accept(codegen.get()))
    // {
    //     fprintf(stderr, "codegen failed: %s\n", err->message.c_str());
    //     return 0;
    // }
}