#ifndef __MOCK_H__
#define __MOCK_H__

#include "lexer/input.h"
#include <gmock/gmock.h>

class MockStdIn : public IInput {
public:
  MOCK_METHOD(int, GetChar, (), (override));
};

// Mock function variables
extern std::string mockInputStr;
extern int mockInputIdx;

// Mock getchar function
int mockGetChar();

#endif