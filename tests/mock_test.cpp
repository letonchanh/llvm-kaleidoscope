#include "mock/mock.h"

TEST(MockStdInTest, GetChar)
{
  MockStdIn mockInput;
  mockInputStr = "Hello";
  mockInputIdx = 0;

  ON_CALL(mockInput, GetChar).WillByDefault([&]()
                                            { return mockGetChar(); });
  EXPECT_CALL(mockInput, GetChar).Times(mockInputStr.length() + 1);

  EXPECT_EQ(mockInput.GetChar(), 'H');
  EXPECT_EQ(mockInput.GetChar(), 'e');
  EXPECT_EQ(mockInput.GetChar(), 'l');
  EXPECT_EQ(mockInput.GetChar(), 'l');
  EXPECT_EQ(mockInput.GetChar(), 'o');
  EXPECT_EQ(mockInput.GetChar(), EOF);
}