#ifndef __TOKEN_H__
#define __TOKEN_H__

// The lexer returns tokens [0-255] if it is an unknown character,
// otherwise one of these for known things.

enum Token
{
    tok_eof = -1,

    // Commands
    tok_def = -2,
    tok_extern = -3,

    // Primary
    tok_identifier = -4,
    tok_number = -5,

    // Control
    tok_if = -6,
    tok_then = -7,
    tok_else = -8,
};

#endif