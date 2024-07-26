load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "lexer_test",
    srcs = ["lexer_test.cpp"],
    deps = [
        "//src/lexer:lexer_lib",
        "//tests/mock:mock_lib",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "parser_test",
    srcs = ["parser_test.cpp"],
    deps = [
        "//src/ast:ast_lib",
        "//src/lexer:lexer_lib",
        "//src/parser:parser_lib",
        "//tests/mock:mock_lib",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "mock_test",
    srcs = ["mock_test.cpp"],
    deps = [
        "//tests/mock:mock_lib",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ],
)
