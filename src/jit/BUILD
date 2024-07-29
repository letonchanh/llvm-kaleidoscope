cc_library(
    name = "jit_lib",
    srcs = ["jit_manager.cpp"],
    hdrs = [
        "jit.h",
        "jit_manager.h",
    ],
    includes = [
        ".",
        "..",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@llvm-project//llvm:Core",
        "@llvm-project//llvm:OrcJIT",
        "@llvm-project//llvm:Support",
    ],
)
