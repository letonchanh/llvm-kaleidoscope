load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "kaleidoscope",
    srcs = ["main.cpp"],
    deps = [
        "//src/ast:ast_lib",
        "//src/codegen:codegen_visitor",
        "//src/lexer:lexer_lib",
        "//src/parser:parser_lib",
        "//src/utils:printer_visitor",
        "@llvm-project//llvm:Core",
        "@llvm-project//llvm:Support",
        # "@llvm-project//llvm:Target",
    ],
)
