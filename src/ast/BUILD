load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "ast_lib",
    hdrs = glob(["**/*.h"]),
    includes = [
        ".",
        "..",
    ],
    visibility = ["//visibility:public"],
    deps = ["//src/visitor:visitor_lib"],
)
