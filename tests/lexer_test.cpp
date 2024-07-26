#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <memory>
#include <string>

#include "lexer/lexer.h"
#include "lexer/token.h"

#include "mock/mock.h"

class LexerTest : public testing::Test {
protected:
  void SetUp(const std::string &input) {
    auto inputPtr = std::make_unique<MockStdIn>();
    mockInput = inputPtr.get();
    lexer = std::make_unique<Lexer>(std::move(inputPtr));

    mockInputStr = input;
    mockInputIdx = 0;

    ON_CALL(*mockInput, GetChar).WillByDefault([&]() { return mockGetChar(); });
  }

  MockStdIn *mockInput;
  std::unique_ptr<Lexer> lexer;
};

TEST_F(LexerTest, Identifier) {
  SetUp("def foo extern");

  EXPECT_CALL(*mockInput, GetChar).Times(mockInputStr.length() + 1);

  EXPECT_EQ(lexer->GetNextToken(), tok_def) << "Failed";
  EXPECT_EQ(lexer->GetNextToken(), tok_identifier);
  EXPECT_EQ(lexer->IdentifierStr, "foo");
  EXPECT_EQ(lexer->GetNextToken(), tok_extern);
  EXPECT_EQ(lexer->GetNextToken(), tok_eof);
}

TEST_F(LexerTest, Comment) {
  SetUp("# This is a comment\nfoo");

  EXPECT_CALL(*mockInput, GetChar).Times(mockInputStr.length() + 1);

  EXPECT_EQ(lexer->GetNextToken(), tok_identifier);
  EXPECT_EQ(lexer->IdentifierStr, "foo");
  EXPECT_EQ(lexer->GetNextToken(), tok_eof);
}

TEST_F(LexerTest, MultiLineComment) {
  SetUp("# This is a comment\n# This is another comment\nfoo");

  EXPECT_CALL(*mockInput, GetChar).Times(mockInputStr.length() + 1);

  EXPECT_EQ(lexer->GetNextToken(), tok_identifier);
  EXPECT_EQ(lexer->IdentifierStr, "foo");
  EXPECT_EQ(lexer->GetNextToken(), tok_eof);
}

TEST_F(LexerTest, GetNum) {
  SetUp("123 123. .123 123.456 123.456.789.123 0456.0789");

  EXPECT_CALL(*mockInput, GetChar).Times(mockInputStr.length() + 1);

  EXPECT_EQ(lexer->GetNextToken(), tok_number);
  EXPECT_EQ(lexer->NumVal, 123);

  EXPECT_EQ(lexer->GetNextToken(), tok_number);
  EXPECT_EQ(lexer->NumVal, 123);

  EXPECT_EQ(lexer->GetNextToken(), tok_number);
  EXPECT_EQ(lexer->NumVal, 0.123);

  EXPECT_EQ(lexer->GetNextToken(), tok_number);
  EXPECT_EQ(lexer->NumVal, 123.456);

  EXPECT_EQ(lexer->GetNextToken(), tok_number);
  EXPECT_EQ(lexer->NumVal, 123.456);

  EXPECT_EQ(lexer->GetNextToken(), tok_number);
  EXPECT_EQ(lexer->NumVal, 0.789);

  EXPECT_EQ(lexer->GetNextToken(), tok_number);
  EXPECT_EQ(lexer->NumVal, 0.123);

  EXPECT_EQ(lexer->GetNextToken(), tok_number);
  EXPECT_EQ(lexer->NumVal, 456.0789);

  EXPECT_EQ(lexer->GetNextToken(), tok_eof);
}
