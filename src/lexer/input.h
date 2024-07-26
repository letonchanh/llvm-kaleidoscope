#ifndef __INPUT_H__
#define __INPUT_H__

// IInput is an interface for the input of lexer (e.g. stdin, mock)
class IInput
{
public:
    virtual ~IInput() = default;
    virtual int GetChar() = 0;
};

#endif