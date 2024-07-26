#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <memory>
#include <string>

#include "lexer/input.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser/parser.h"

#include "ast/BinaryExprAST.h"
#include "ast/CallExprAST.h"
#include "ast/NumberExprAST.h"
#include "ast/VariableExprAST.h"

#include "mock/mock.h"

class ParserTest : public testing::Test
{
protected:
  void SetUp(const std::string &input)
  {

    std::unique_ptr<MockStdIn> inputPtr = std::make_unique<MockStdIn>();
    mockInput = inputPtr.get();

    mockInputStr = input;
    mockInputIdx = 0;

    ON_CALL(*mockInput, GetChar).WillByDefault([&]()
                                               { return mockGetChar(); });

    lexer = std::make_unique<Lexer>(std::move(inputPtr));
    lexer->GetNextToken();
    parser = std::make_unique<Parser>(std::move(lexer));
  }

  MockStdIn *mockInput;
  std::unique_ptr<Lexer> lexer;
  std::unique_ptr<Parser> parser;
};

TEST_F(ParserTest, NumberExpr)
{
  SetUp("123.456");

  auto ast = parser->parseNumberExpr();

  NumberExprAST *ptr = dynamic_cast<NumberExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ(123.456, ptr->Val);
}

TEST_F(ParserTest, VariableExpr)
{
  SetUp("foo 123");
  auto ast = parser->parseIdentifierExpr();
  VariableExprAST *ptr = dynamic_cast<VariableExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ("foo", ptr->Name);
}

TEST_F(ParserTest, CallExpr)
{
  SetUp("func0()");
  auto ast = parser->parseIdentifierExpr();
  CallExprAST *ptr = dynamic_cast<CallExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ("func0", ptr->Callee);
  EXPECT_EQ(0, ptr->Args.size());

  SetUp("func1(x)");
  ast = parser->parseIdentifierExpr();
  ptr = dynamic_cast<CallExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ("func1", ptr->Callee);
  EXPECT_EQ(1, ptr->Args.size());

  SetUp("func2(3, x)");
  ast = parser->parseIdentifierExpr();
  ptr = dynamic_cast<CallExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ("func2", ptr->Callee);
  EXPECT_EQ(2, ptr->Args.size());

  SetUp("func3(3, (x), y)");
  ast = parser->parseIdentifierExpr();
  ptr = dynamic_cast<CallExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ("func3", ptr->Callee);
  EXPECT_EQ(3, ptr->Args.size());

  SetUp("func0(");
  ast = parser->parseIdentifierExpr();
  EXPECT_EQ(nullptr, ast);

  SetUp("func0)");
  ast = parser->parseIdentifierExpr();
  EXPECT_NE(nullptr, ast);

  SetUp("func0(())");
  ast = parser->parseIdentifierExpr();
  EXPECT_EQ(nullptr, ast);

  SetUp("func0(,)");
  ast = parser->parseIdentifierExpr();
  EXPECT_EQ(nullptr, ast);

  SetUp("func1(extern)");
  ast = parser->parseIdentifierExpr();
  EXPECT_EQ(nullptr, ast);

  SetUp("func2(x,");
  ast = parser->parseIdentifierExpr();
  EXPECT_EQ(nullptr, ast);

  SetUp("func2(x,)");
  ast = parser->parseIdentifierExpr();
  EXPECT_EQ(nullptr, ast);

  SetUp("func2(extern,)");
  ast = parser->parseIdentifierExpr();
  EXPECT_EQ(nullptr, ast);

  SetUp("func2((x),)");
  ast = parser->parseIdentifierExpr();
  EXPECT_EQ(nullptr, ast);
}

TEST_F(ParserTest, ParenExpr)
{
  SetUp("(x)");
  auto ast = parser->parseParenExpr();
  VariableExprAST *ptr1 = dynamic_cast<VariableExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr1);
  EXPECT_EQ("x", ptr1->Name);

  SetUp("(1.0)");
  ast = parser->parseParenExpr();
  NumberExprAST *ptr2 = dynamic_cast<NumberExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr2);
  EXPECT_EQ(1.0, ptr2->Val);

  SetUp("(x");
  ast = parser->parseParenExpr();
  EXPECT_EQ(nullptr, ast);
}

TEST_F(ParserTest, PrimaryExpr)
{
  SetUp("123.456");
  auto ast = parser->parsePrimaryExpr();
  NumberExprAST *ptr1 = dynamic_cast<NumberExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr1);
  EXPECT_EQ(123.456, ptr1->Val);

  SetUp("func1(x)");
  ast = parser->parsePrimaryExpr();
  CallExprAST *ptr2 = dynamic_cast<CallExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr2);
  EXPECT_EQ("func1", ptr2->Callee);
  EXPECT_EQ(1, ptr2->Args.size());

  SetUp("(func1((x)))");
  ast = parser->parsePrimaryExpr();
  CallExprAST *ptr3 = dynamic_cast<CallExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr3);
  EXPECT_EQ("func1", ptr3->Callee);
  EXPECT_EQ(1, ptr3->Args.size());
}

TEST_F(ParserTest, Expression)
{
  SetUp("x + y*2*z - z + 2*(x + y) < x * z");
  std::unique_ptr<ExprAST> ast = parser->parseExpression();
  BinaryExprAST *ptr = dynamic_cast<BinaryExprAST *>(ast.get());
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ('<', ptr->Op);
}

TEST_F(ParserTest, Prototype)
{
  SetUp("fib(x)");
  std::unique_ptr<PrototypeAST> ast = parser->parsePrototype();
  EXPECT_NE(nullptr, ast);
  EXPECT_EQ("fib", ast->Name);
  EXPECT_EQ(1, ast->Args.size());

  SetUp("func0()");
  ast = parser->parsePrototype();
  EXPECT_NE(nullptr, ast);
  EXPECT_EQ("func0", ast->Name);
  EXPECT_EQ(0, ast->Args.size());

  SetUp("func2(x y)");
  ast = parser->parsePrototype();
  EXPECT_NE(nullptr, ast);
  EXPECT_EQ("func2", ast->Name);
  EXPECT_EQ(2, ast->Args.size());

  SetUp("func3(x y z)");
  ast = parser->parsePrototype();
  EXPECT_NE(nullptr, ast);
  EXPECT_EQ("func3", ast->Name);
  EXPECT_EQ(3, ast->Args.size());

  SetUp("func2(x y");
  ast = parser->parsePrototype();
  EXPECT_EQ(nullptr, ast);

  SetUp("func2(x def");
  ast = parser->parsePrototype();
  EXPECT_EQ(nullptr, ast);
}

TEST_F(ParserTest, Extern)
{
  SetUp("extern fib(x)");
  std::unique_ptr<PrototypeAST> ast = parser->parseExtern();
  EXPECT_NE(nullptr, ast);
}

TEST_F(ParserTest, Definition)
{
  SetUp("def inc(x) x + 1");
  std::unique_ptr<FunctionAST> ast = parser->parseDefinition();
  EXPECT_NE(nullptr, ast);
}