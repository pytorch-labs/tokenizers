// swift-tools-version: 5.7
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "Tokenizers",
    platforms: [
      .iOS(.v17),
      .macOS(.v12),
    ],
    products: [
        .library(
            name: "Tokenizers",
            targets: ["Tokenizers"]
        ),
    ],
    dependencies: [
        // Add external dependencies here if needed
    ],
    targets: [
        .target(
            name: "Tokenizers",
            dependencies: [
                "AbseilCpp",
                "RE2",
                "SentencePiece",
                "LlamaCppUnicode"
            ],
            path: ".",
            sources: [
                "src/bpe_tokenizer_base.cpp",
                "src/hf_tokenizer.cpp",
                "src/llama2c_tokenizer.cpp",
                "src/normalizer.cpp",
                "src/pre_tokenizer.cpp",
                "src/re2_regex.cpp",
                "src/regex.cpp",
                "src/sentencepiece.cpp",
                "src/tiktoken.cpp",
                "src/token_decoder.cpp"
            ],
            publicHeadersPath: "include",
            cxxSettings: [
                .headerSearchPath("include"),
                .headerSearchPath("third-party/sentencepiece"),
                .headerSearchPath("third-party/sentencepiece/src"),
                .headerSearchPath("third-party/re2"),
                .headerSearchPath("third-party/json/single_include"),
                .headerSearchPath("third-party/llama.cpp-unicode/include"),
                .define("ABSL_ENABLE_INSTALL"),
                .define("ABSL_PROPAGATE_CXX_STD"),
                .unsafeFlags(["-Wno-attributes"]),
                .cxxLanguageStandard(.cxx17)
            ]
        ),

        // Abseil C++ library
        .target(
            name: "AbseilCpp",
            path: "third-party/abseil-cpp",
            publicHeadersPath: ".",
            cxxSettings: [
                .cxxLanguageStandard(.cxx17)
            ]
        ),

        // RE2 library
        .target(
            name: "RE2",
            path: "third-party/re2",
            publicHeadersPath: ".",
            cxxSettings: [
                .cxxLanguageStandard(.cxx17)
            ]
        ),

        // SentencePiece library
        .target(
            name: "SentencePiece",
            path: "third-party/sentencepiece",
            publicHeadersPath: "src",
            cxxSettings: [
                .headerSearchPath("src"),
                .headerSearchPath("third_party/protobuf-lite"),
                .cxxLanguageStandard(.cxx17)
            ]
        ),

        // Llama.cpp Unicode library
        .target(
            name: "LlamaCppUnicode",
            path: "third-party/llama.cpp-unicode",
            sources: [
                "src/unicode.cpp",
                "src/unicode-data.cpp"
            ],
            publicHeadersPath: "include",
            cxxSettings: [
                .cxxLanguageStandard(.cxx17)
            ]
        )
    ],
    cxxLanguageStandard: .cxx17
)
