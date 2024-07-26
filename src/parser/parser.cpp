#include <iostream>

#include "lexer/token.h"

#include "ast/NumberExprAST.h"
#include "ast/VariableExprAST.h"
#include "ast/CallExprAST.h"
#include "ast/BinaryExprAST.h"

#include "parser.h"
#include "logger.h"

// This routine expects to be called when the current token is a tok_number.
// It takes the current number value and creates a NumberExprAST node.
std::unique_ptr<ExprAST> Parser::parseNumberExpr()
{
    auto result = std::make_unique<NumberExprAST>(lexer->NumVal);
    lexer->GetNextToken();
    return std::move(result);
}

// This routine parses expressions in "(" and ")" characters
std::unique_ptr<ExprAST> Parser::parseParenExpr()
{
    lexer->GetNextToken(); // eat '('
    auto v = parseExpression();
    if (!v)
        return nullptr;
    if (lexer->CurTok != ')')
        return ParseError("expected ')'");
    lexer->GetNextToken(); // eat ')'
    return v;
}

/// identifierExpr
///   ::= identifier
///     | identifier '(' expression* ')'
std::unique_ptr<ExprAST> Parser::parseIdentifierExpr()
{
    std::string idName = lexer->IdentifierStr;
    lexer->GetNextToken(); // eat identifier

    if (lexer->CurTok != '(')
        return std::make_unique<VariableExprAST>(idName);

    lexer->GetNextToken(); // eat '('
    std::vector<std::unique_ptr<ExprAST>> args;
    if (lexer->CurTok != ')')
    {
        while (true)
        {
            if (auto arg = parseExpression())
                args.push_back(std::move(arg));
            else
                return nullptr;

            if (lexer->CurTok == ')')
                break;

            if (lexer->CurTok != ',')
                return ParseError("Expected ')' or ',' after an argument of a call expression");
            lexer->GetNextToken(); // eat ','
        }
    }

    lexer->GetNextToken(); // eat ')'
    return std::make_unique<CallExprAST>(idName, std::move(args));
}

/// PrimaryExpr
///   ::= IdentifierExpr
///     | NumberExpr
///     | ParenExpr
std::unique_ptr<ExprAST> Parser::parsePrimaryExpr()
{
    switch (lexer->CurTok)
    {
    case tok_identifier:
        return parseIdentifierExpr();
    case tok_number:
        return parseNumberExpr();
    case '(':
        return parseParenExpr();
    default:
        std::ostringstream errorMsg;
        errorMsg << "unknown token " << lexer->CurTok << " when expecting a primary expression";
        return ParseError(errorMsg.str().c_str());
    }
}

/// Expression
///   ::= PrimaryExpr BinOpRHS
std::unique_ptr<ExprAST> Parser::parseExpression()
{
    auto LHS = parsePrimaryExpr();
    if (!LHS)
        return nullptr;
    return parseBinOpRHS(0, std::move(LHS));
}

/// BinOpRHS
///   ::= '<' Expression
///     | '+-' Expression
///     | '*/' Expression
std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> LHS)
{
    while (true)
    {
        int tokPrec = getTokPrecedence(lexer->CurTok);

        // If this is a binop that binds at least as tightly as the current binop,
        // (tokPrec >= exprPrec) consume it, otherwise we are done.
        if (tokPrec < exprPrec)
            return LHS;

        int binOp = lexer->CurTok;
        lexer->GetNextToken(); // eat binop

        // Parse the primary expression after the binary operator.
        auto RHS = parsePrimaryExpr();
        if (!RHS)
            return nullptr;

        // If binop binds less tightly with RHS than the operator after RHS
        // (tokPrec < nextPrec), let the pending operator take RHS as its LHS.
        int nextPrec = getTokPrecedence(lexer->CurTok);
        if (tokPrec < nextPrec)
        {
            RHS = parseBinOpRHS(tokPrec + 1, std::move(RHS));
        }

        LHS = std::make_unique<BinaryExprAST>(binOp, std::move(LHS), std::move(RHS));
    }
}

/// prototype
///   ::= id '(' id* ')'
std::unique_ptr<PrototypeAST> Parser::parsePrototype()
{
    if (lexer->CurTok != tok_identifier)
        return ParseErrorProto("expected function name in prototype");

    std::string fnName = lexer->IdentifierStr;
    lexer->GetNextToken(); // eat id

    if (lexer->CurTok != '(')
        return ParseErrorProto("expected '(' in prototype");
    lexer->GetNextToken(); // eat '('

    // Read the list of argument names.
    std::vector<std::string> argNames;
    while (lexer->CurTok == tok_identifier)
    {
        argNames.push_back(lexer->IdentifierStr);
        lexer->GetNextToken();

        if (lexer->CurTok == ')')
            break;
        if (lexer->CurTok == ',')
            lexer->GetNextToken(); // eat ','
        else
            return ParseErrorProto("expected ')' or ',' after a prototype's argument");
    }

    if (lexer->CurTok != ')')
        return ParseErrorProto("expected ')' in prototype");
    lexer->GetNextToken(); // eat ')'

    return std::make_unique<PrototypeAST>(fnName, argNames);
}

/// definition ::= 'def' prototype expression
std::unique_ptr<FunctionAST> Parser::parseDefinition()
{
    lexer->GetNextToken(); // eat 'def'
    auto proto = parsePrototype();
    if (!proto)
        return nullptr;

    if (auto body = parseExpression())
        return std::make_unique<FunctionAST>(
            std::move(proto),
            std::move(body));
    return nullptr;
}

/// external ::= 'extern' prototype
std::unique_ptr<PrototypeAST> Parser::parseExtern()
{
    lexer->GetNextToken(); // eat 'extern'
    return parsePrototype();
}

/// toplevelexpr ::= expression
std::unique_ptr<FunctionAST> Parser::parseTopLevelExpr()
{
    if (auto expr = parseExpression())
    {
        auto proto = std::make_unique<PrototypeAST>("__main__", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(proto), std::move(expr));
    }
    return nullptr;
}

/// program := (definition | external | toplevelexpr | ';')*
std::unique_ptr<ProgramAST> Parser::ParseProgram(
    const std::vector<Visitor *> &visitors)
{
    auto nodes = std::vector<std::unique_ptr<AST>>();
    while (true)
    {
        fprintf(stderr, "ready> ");
        std::unique_ptr<AST> node = nullptr;
        switch (lexer->CurTok)
        {
        case tok_eof:
            return std::make_unique<ProgramAST>(std::move(nodes));
        case ';':
            lexer->GetNextToken();
            break;
        case tok_def:
            node = parseDefinition();
            break;
        case tok_extern:
            node = parseExtern();
            break;
        default:
            node = parseTopLevelExpr();
            break;
        }
        if (node)
        {
            bool ok = true;
            for (const auto &visitor : visitors)
            {
                if (auto err = node->accept(visitor))
                {
                    fprintf(stderr, "visitor failed: %s\n", err->message.c_str());
                    ok = false;
                }
            }
            if (ok)
                nodes.push_back(std::move(node));
        }
    }
    return nullptr;
}
