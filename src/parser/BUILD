load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "parser_lib",
    srcs = glob(["**/*.cpp"]),
    hdrs = glob(["**/*.h"]),
    includes = [
        ".",
        "..",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/ast:ast_lib",
        "//src/lexer:lexer_lib",
        "//src/logger:logger_lib",
        "//src/visitor:visitor_lib",
        "@com_google_googletest//:gtest",
    ],
)
