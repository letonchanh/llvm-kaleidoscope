#include "mock.h"

// Mock function variables
std::string mockInputStr;
int mockInputIdx;

// Mock getchar function
int mockGetChar() {
  if (mockInputStr[mockInputIdx] == '\0') {
    // End of string,
    // EOF should not be returned here, otherwise,
    // only the first test works as LastChar stucks at EOF.
    return EOF;
  }
  return mockInputStr[mockInputIdx++];
}