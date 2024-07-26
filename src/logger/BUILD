load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "logger_lib",
    srcs = glob(["**/*.cpp"]),
    hdrs = glob(["**/*.h"]),
    includes = [
        ".",
        "..",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/ast:ast_lib",
        "@llvm-project//llvm:Core",
        "@llvm-project//llvm:Support",
    ],
)
