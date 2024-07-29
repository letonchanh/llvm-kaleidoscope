load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "printer_visitor",
    srcs = ["printer.cpp"],
    hdrs = ["printer.h"],
    includes = [
        ".",
        "..",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/ast:ast_lib",
        "//src/visitor:visitor_lib",
    ],
)

cc_library(
    name = "error_lib",
    hdrs = ["error.h"],
    includes = [
        ".",
        "..",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "result_lib",
    hdrs = ["result.h"],
    includes = [
        ".",
        "..",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "utils_lib",
    includes = [
        ".",
        "..",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":error_lib",
        ":result_lib",
    ],
)
