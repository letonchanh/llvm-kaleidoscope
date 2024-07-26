#ifndef __LEXER_H__
#define __LEXER_H__

#include <string>
#include "input.h"

class Lexer
{
public:
    // If the current token is an identifier
    // IdentifierStr will hold the name of the identifier
    std::string IdentifierStr;

    // If the current token is a numeric literal
    // NumVal holds its value
    double NumVal;

    /// CurTok/getNextToken - Provide a simple token buffer.
    /// CurTok is the current token the parser is looking at.
    /// getNextToken reads another token from the
    /// lexer and updates CurTok with its results.
    int CurTok;
    int GetNextToken();

    Lexer(std::unique_ptr<IInput> input) : input(std::move(input)) {}

private:
    std::unique_ptr<IInput> input;

    // gettok works by calling the C getchar() function to read characters
    // one at a time from standard input. It eats them as it recognizes them
    // and stores the last character read, but not processed, in LastChar.
    int lastChar = ' ';
    int gettok();
};

#endif