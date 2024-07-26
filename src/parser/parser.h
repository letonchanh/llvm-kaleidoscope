#ifndef __PARSER_H__
#define __PARSER_H__

#include <memory>
#include <map>

#include "gtest/gtest.h"

#include "ast/ExprAST.h"
#include "ast/PrototypeAST.h"
#include "ast/FunctionAST.h"
#include "ast/ProgramAST.h"

#include "visitor/visitor.h"

#include "lexer/lexer.h"

class Parser
{
    std::unique_ptr<Lexer> lexer;
    std::map<char, int> binOpPrecedence;

    int getTokPrecedence(int tok)
    {
        if (!isascii(tok))
            return -1;
        if (binOpPrecedence.count(tok))
            return binOpPrecedence[tok];
        return -1;
    }

    std::unique_ptr<ExprAST> parseNumberExpr();
    std::unique_ptr<ExprAST> parseParenExpr();
    std::unique_ptr<ExprAST> parseIdentifierExpr();
    std::unique_ptr<ExprAST> parsePrimaryExpr();
    std::unique_ptr<ExprAST> parseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);
    std::unique_ptr<ExprAST> parseExpression();
    std::unique_ptr<PrototypeAST> parsePrototype();

    std::unique_ptr<FunctionAST> parseDefinition();
    std::unique_ptr<PrototypeAST> parseExtern();
    std::unique_ptr<FunctionAST> parseTopLevelExpr();

    FRIEND_TEST(ParserTest, NumberExpr);
    FRIEND_TEST(ParserTest, ParenExpr);
    FRIEND_TEST(ParserTest, VariableExpr);
    FRIEND_TEST(ParserTest, CallExpr);
    FRIEND_TEST(ParserTest, PrimaryExpr);
    FRIEND_TEST(ParserTest, Expression);
    FRIEND_TEST(ParserTest, Prototype);
    FRIEND_TEST(ParserTest, Extern);
    FRIEND_TEST(ParserTest, Definition);

public:
    Parser(std::unique_ptr<Lexer> lexer) : lexer(std::move(lexer))
    {
        // Install standard binary operators.
        // 1 is lowest precedence.
        binOpPrecedence['<'] = 10;
        binOpPrecedence['+'] = 20;
        binOpPrecedence['-'] = 20;
        binOpPrecedence['*'] = 40; // highest
    }

    std::unique_ptr<ProgramAST> ParseProgram(
        const std::vector<Visitor *> &visitors = std::vector<Visitor *>());
};

#endif