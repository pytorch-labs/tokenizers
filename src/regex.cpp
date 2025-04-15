#include "pytorch/tokenizers/regex.h"
#include "pytorch/tokenizers/re2_regex.h"
#include "pytorch/tokenizers/std_regex.h"
#include "pytorch/tokenizers/pcre2_regex.h"

#include <re2/re2.h>
#include <iostream>
#include <memory>

/**
 * @brief Factory function that creates a regex object using RE2 if possible.
 *        Falls back to PCRE2 if RE2 rejects the pattern, then to std::regex if PCRE2 fails.
 */
std::unique_ptr<IRegex> createRegex(const std::string& pattern) {
  // Try RE2 first
  auto re2 = std::make_unique<Re2Regex>("(" + pattern + ")");

  if (re2->ok()) {
    return re2;
  }

  const re2::RE2* raw = re2->rawRegex();
  if (raw && raw->error_code() == re2::RE2::ErrorBadPerlOp) {
    // RE2 doesn't support some Perl features, try PCRE2
    auto pcre2 = std::make_unique<Pcre2Regex>("(" + pattern + ")");
    
    if (pcre2->ok()) {
      std::cout << "RE2 is unable to support things such as negative lookaheads in "
                << pattern << ", using PCRE2 instead.";
      return pcre2;
    }
    
    // If PCRE2 also fails, fall back to std::regex
    try {
      std::cout << "PCRE2 failed to compile pattern, falling back to std::regex.";
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
