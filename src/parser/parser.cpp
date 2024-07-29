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
ParseResult Parser::parseNumberExpr()
{
    auto v = std::make_unique<NumberExprAST>(lexer->NumVal);
    lexer->GetNextToken();
    return ParseResult(std::move(v));
}

// This routine parses expressions in "(" and ")" characters
ParseResult Parser::parseParenExpr()
{
    lexer->GetNextToken(); // eat '('
    auto r = parseExpression();
    if (r.isError())
        return r;
    if (lexer->CurTok != ')')
        return ParseResult(Error("expected ')'"));
    lexer->GetNextToken(); // eat ')'
    return r;
}

/// identifierExpr
///   ::= identifier
///     | identifier '(' expression* ')'
ParseResult Parser::parseIdentifierExpr()
{
    std::string idName = lexer->IdentifierStr;
    lexer->GetNextToken(); // eat identifier

    if (lexer->CurTok != '(')
    {
        return ParseResult(std::make_unique<VariableExprAST>(idName));
    }

    lexer->GetNextToken(); // eat '('
    std::vector<std::unique_ptr<ExprAST>> args;
    if (lexer->CurTok != ')')
    {
        while (true)
        {
            auto r = parseExpression();
            if (r.isError())
                return r;
            std::unique_ptr<AST> v = r.value();
            std::unique_ptr<ExprAST> arg(static_cast<ExprAST *>(v.release()));
            args.push_back(std::move(arg));

            if (lexer->CurTok == ')')
                break;

            if (lexer->CurTok != ',')
                return ParseResult(Error("Expected ')' or ',' after an argument of a call expression"));
            lexer->GetNextToken(); // eat ','
        }
    }

    lexer->GetNextToken(); // eat ')'
    return ParseResult(std::make_unique<CallExprAST>(idName, std::move(args)));
}

/// PrimaryExpr
///   ::= IdentifierExpr
///     | NumberExpr
///     | ParenExpr
ParseResult Parser::parsePrimaryExpr()
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
        return ParseResult(Error(errorMsg.str()));
    }
}

/// Expression
///   ::= PrimaryExpr BinOpRHS
ParseResult Parser::parseExpression()
{
    auto r = parsePrimaryExpr();
    if (r.isError())
        return r;
    std::unique_ptr<AST> v = r.value();
    std::unique_ptr<ExprAST> LHS(static_cast<ExprAST *>(v.release()));
    return parseBinOpRHS(0, std::move(LHS));
}

/// BinOpRHS
///   ::= '<' Expression
///     | '+-' Expression
///     | '*/' Expression
ParseResult Parser::parseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> LHS)
{
    while (true)
    {
        int tokPrec = getTokPrecedence(lexer->CurTok);

        // If this is a binop that binds at least as tightly as the current binop,
        // (tokPrec >= exprPrec) consume it, otherwise we are done.
        if (tokPrec < exprPrec)
            return ParseResult(std::move(LHS));

        int binOp = lexer->CurTok;
        lexer->GetNextToken(); // eat binop

        // Parse the primary expression after the binary operator.
        auto r = parsePrimaryExpr();
        if (r.isError())
            return r;

        std::unique_ptr<AST> v = r.value();
        std::unique_ptr<ExprAST> RHS(static_cast<ExprAST *>(v.release()));

        // If binop binds less tightly with RHS than the operator after RHS
        // (tokPrec < nextPrec), let the pending operator take RHS as its LHS.
        int nextPrec = getTokPrecedence(lexer->CurTok);
        if (tokPrec < nextPrec)
        {
            r = parseBinOpRHS(tokPrec + 1, std::move(RHS));
            if (r.isError())
                return r;

            v = r.value();
            RHS = std::unique_ptr<ExprAST>(static_cast<ExprAST *>(v.release()));
        }

        LHS = std::make_unique<BinaryExprAST>(binOp, std::move(LHS), std::move(RHS));
    }
}

/// prototype
///   ::= id '(' id* ')'
ParseResult Parser::parsePrototype()
{
    if (lexer->CurTok != tok_identifier)
        return ParseResult(Error("expected function name in prototype"));

    std::string fnName = lexer->IdentifierStr;
    lexer->GetNextToken(); // eat id

    if (lexer->CurTok != '(')
        return ParseResult(Error("expected '(' in prototype"));
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
            return ParseResult(Error("expected ')' or ',' after a prototype's argument"));
    }

    if (lexer->CurTok != ')')
        return ParseResult(Error("expected ')' in prototype"));
    lexer->GetNextToken(); // eat ')'

    return ParseResult(std::make_unique<PrototypeAST>(fnName, argNames));
}

/// definition ::= 'def' prototype expression
ParseResult Parser::parseDefinition()
{
    lexer->GetNextToken(); // eat 'def'
    auto r = parsePrototype();
    if (r.isError())
        return r;
    auto v = r.value();
    std::unique_ptr<PrototypeAST> proto(static_cast<PrototypeAST *>(v.release()));

    r = parseExpression();
    if (r.isError())
        return r;
    v = r.value();
    std::unique_ptr<ExprAST> body(static_cast<ExprAST *>(v.release()));

    return ParseResult(std::make_unique<FunctionAST>(
        std::move(proto),
        std::move(body)));
}

/// external ::= 'extern' prototype
ParseResult Parser::parseExtern()
{
    lexer->GetNextToken(); // eat 'extern'
    return parsePrototype();
}

/// toplevelexpr ::= expression
ParseResult Parser::parseTopLevelExpr()
{
    auto r = parseExpression();
    if (r.isError())
        return r;
    auto v = r.value();
    std::unique_ptr<ExprAST> expr(static_cast<ExprAST *>(v.release()));

    auto proto = std::make_unique<PrototypeAST>("__main__", std::vector<std::string>());
    return ParseResult(std::make_unique<FunctionAST>(std::move(proto), std::move(expr)));
}

/// program := (definition | external | toplevelexpr | ';')*
ParseResult Parser::ParseProgram(
    const std::vector<Visitor *> &visitors)
{
    ParseResult r(Error("dummy", -1));
    auto nodes = std::vector<std::unique_ptr<AST>>();
    while (true)
    {
        switch (lexer->CurTok)
        {
        case tok_eof:
            return ParseResult(std::make_unique<ProgramAST>(std::move(nodes)));
        case ';':
            r = ParseResult(Error("dummy", -1)); // a dummy error
            break;
        case tok_def:
            r = parseDefinition();
            break;
        case tok_extern:
            r = parseExtern();
            break;
        default:
            r = parseTopLevelExpr();
            break;
        }

        if (r.isError())
        {
            Error err = r.error();
            if (err.code >= 0)
            {
                fprintf(stderr, "error: %s\n", err.message.c_str());
                fprintf(stderr, "ready> ");
            }
            // Move to the next token for error recovery
            lexer->GetNextToken();
            continue;
        }

        std::unique_ptr<AST> node = r.value();

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
        fprintf(stderr, "ready> ");
    }
}
