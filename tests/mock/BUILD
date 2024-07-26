load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "mock_lib",
    srcs = glob(["**/*.cpp"]),
    hdrs = glob(["**/*.h"]),
    includes = ["../../src"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/lexer:lexer_lib",
        "@com_google_googletest//:gtest",
    ],
)
