#include "lexer.h"
#include "token.h"
#include "input.h"

// The actual implementation of the lexer is a single function named gettok.
// The gettok function is called to return the next token from standard input.
int Lexer::gettok()
{
    // The first thing that it has to do is ignore whitespace between tokens.
    while (isspace(lastChar))
    {
        lastChar = input->GetChar();
    }

    // The next thing gettok needs to do is recognize identifiers and specific keywords like "def".
    // Identifier: [a-zA-Z][a-zA-Z0-9]*
    if (isalpha(lastChar))
    {
        IdentifierStr = lastChar;

        while (isalnum(lastChar = input->GetChar()))
        {
            IdentifierStr += lastChar;
        }

        if (IdentifierStr == "def")
            return tok_def;
        if (IdentifierStr == "extern")
            return tok_extern;
        if (IdentifierStr == "if")
            return tok_if;
        if (IdentifierStr == "then")
            return tok_then;
        if (IdentifierStr == "else")
            return tok_else;
        return tok_identifier;
    }

    // Number: [0-9]*(.)?[0-9]*
    if (isnumber(lastChar) || lastChar == '.')
    {
        std::string NumStr;

        if (isnumber(lastChar))
        {
            do
            {
                NumStr += lastChar;
                lastChar = input->GetChar();
            } while (isnumber(lastChar));
        }
        if (lastChar == '.')
        {
            do
            {
                NumStr += lastChar;
                lastChar = input->GetChar();
            } while (isnumber(lastChar));
        }

        NumVal = strtod(NumStr.c_str(), 0);
        return tok_number;
    }

    // Line comment
    if (lastChar == '#')
    {
        do
            lastChar = input->GetChar();
        while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

        if (lastChar != EOF)
            return gettok();
    }

    // EOF
    if (lastChar == EOF)
        return tok_eof;

    // Otherwise, just return the character as its ascii value
    int ThisChar = lastChar;
    lastChar = input->GetChar();
    return ThisChar;
}

int Lexer::GetNextToken()
{
    return CurTok = gettok();
}
