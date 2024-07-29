cc_library(
    name = "optimizer_lib",
    srcs = ["optimizer.cpp"],
    hdrs = ["optimizer.h"],
    includes = [
        ".",
        "..",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@llvm-project//llvm:Analysis",
        "@llvm-project//llvm:Core",
        "@llvm-project//llvm:InstCombine",
        "@llvm-project//llvm:Passes",
        "@llvm-project//llvm:Scalar",
        "@llvm-project//llvm:Support",
    ],
)

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
        ":optimizer_lib",
        "//src/ast:ast_lib",
        "//src/jit:jit_lib",
        "//src/logger:logger_lib",
        "//src/utils:utils_lib",
        "//src/visitor:visitor_lib",
        "@llvm-project//llvm:AllTargetsAsmParsers",  # JIT
        "@llvm-project//llvm:AllTargetsCodeGens",  # JIT
        "@llvm-project//llvm:Core",
        "@llvm-project//llvm:Support",
    ],
)
