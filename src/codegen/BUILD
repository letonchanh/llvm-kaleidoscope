cc_library(
    name = "codegen_visitor",
    srcs = ["codegen.cpp"],
    hdrs = ["codegen.h"],
    includes = [
        ".",
        "..",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/ast:ast_lib",
        "//src/logger:logger_lib",
        "//src/visitor:visitor_lib",
        "@llvm-project//llvm:Core",
        "@llvm-project//llvm:Support",
    ],
)
