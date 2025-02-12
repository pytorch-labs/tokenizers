load("@fbsource//tools/build_defs:glob_defs.bzl", "subdir_glob")
load("@fbsource//xplat/executorch/build:runtime_wrapper.bzl", "runtime")

def define_common_targets():
    """Defines targets that should be shared between fbcode and xplat.

    The directory containing this targets.bzl file should also contain both
    TARGETS and BUCK files that call this function.
    """

    runtime.cxx_library(
        name = "headers",
        exported_headers = subdir_glob([
            ("include", "*.h"),
            ("include", "**/*.h"),
        ]),
        header_namespace = "",
        visibility = [
            "@EXECUTORCH_CLIENTS",
        ],
    )

    runtime.cxx_library(
        name = "sentencepiece",
        srcs = [
            "src/sentencepiece.cpp",
        ],
        exported_deps = [
            ":headers",
        ],
        visibility = [
            "@EXECUTORCH_CLIENTS",
        ],
        exported_external_deps = [
            "sentencepiece",
        ],
    )

    runtime.cxx_library(
        name = "tiktoken",
        srcs = [
            "src/tiktoken.cpp",
            "src/bpe_tokenizer_base.cpp",
        ],
        exported_deps = [
            ":headers",
        ],
        visibility = [
            "@EXECUTORCH_CLIENTS",
        ],
        exported_external_deps = [
            "re2",
        ],
    )

    runtime.cxx_library(
        name = "hf_tokenizer",
        srcs = [
            "src/hf_tokenizer.cpp",
            "src/bpe_tokenizer_base.cpp",
        ],
        exported_deps = [
            ":headers",
        ],
        visibility = [
            "@EXECUTORCH_CLIENTS",
        ],
        exported_external_deps = [
            "re2",
            "nlohmann_json",
        ],
    )
