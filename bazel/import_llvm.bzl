"""Provides the repository macro to import LLVM."""

# load(
#     "@bazel_tools//tools/build_defs/repo:git.bzl",
#     "new_git_repository",
# )

def import_llvm(name):
    """Imports LLVM."""
    # LLVM_COMMIT = "ea5f74b3053d1aea376f860a08903495a47fff9b"  # "ce811fb6d94e1d4af1fd1f52fbf109bc34834970"

    # new_git_repository(
    #     name = name,
    #     # this BUILD file is intentionally empty, because the LLVM project
    #     # internally contains a set of bazel BUILD files overlaying the project.
    #     build_file_content = "# empty",
    #     commit = LLVM_COMMIT,
    #     init_submodules = False,
    #     remote = "https://github.com/letonchanh/llvm-project.git",
    # )

    native.new_local_repository(
        name = name,
        build_file_content = "# empty",
        path = "third_party/llvm-project",
    )
