load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "visitor_lib",
    hdrs = ["visitor.h"],
    includes = [
        ".",
        "..",
    ],
    visibility = ["//visibility:public"],
    deps = ["//src/utils:utils_lib"],
)
