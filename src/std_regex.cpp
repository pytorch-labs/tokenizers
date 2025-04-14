#include "pytorch/tokenizers/std_regex.h"
#include <regex>

StdRegex::StdRegex(const std::string& pattern)
    : regex_("(" + pattern + ")") // Add parentheses like RE2 version
{}

std::vector<Match> StdRegex::findAll(const std::string& text) const {
  std::vector<Match> result;
  std::sregex_iterator iter(text.begin(), text.end(), regex_);
  std::sregex_iterator end;

  for (; iter != end; ++iter) {
    const auto& match = *iter;
    result.push_back({
        match[1].str(), // capture group 1
        static_cast<size_t>(match.position(1)) // position of group 1
    });
  }

  return result;
}
