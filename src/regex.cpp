#include "pytorch/tokenizers/regex.h"
#include "pytorch/tokenizers/re2_regex.h"
#include "pytorch/tokenizers/std_regex.h"

#include <re2/re2.h>
#include <iostream>
#include <memory>

/**
 * @brief Factory function that creates a regex object using RE2 if possible.
 *        Falls back to std::regex if RE2 rejects the pattern with
 *        ErrorBadPerlOp.
 */
std::unique_ptr<IRegex> createRegex(const std::string& pattern) {
  auto re2 = std::make_unique<Re2Regex>("(" + pattern + ")");

  if (re2->ok()) {
    return re2;
  }

  const re2::RE2* raw = re2->rawRegex();
  if (raw && raw->error_code() == re2::RE2::ErrorBadPerlOp) {
    try {
      std::cout
          << "RE2 is unable to support things such as negative lookaheads in "
          << pattern << ", defaulting to std::regex.";
      return std::make_unique<StdRegex>("(" + pattern + ")");
    } catch (const std::regex_error& e) {
      std::cerr << "std::regex failed: " << e.what() << std::endl;
      return nullptr;
    }
  } else {
    std::cerr << "RE2 failed to compile pattern: " << pattern << "\n";
    std::cerr << "Error: " << (raw ? raw->error() : "unknown") << std::endl;
    return nullptr;
  }
}
