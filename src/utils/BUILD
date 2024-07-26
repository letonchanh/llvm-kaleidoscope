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
