/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <pytorch/tokenizers/std_regex.h>
#include <regex>

namespace tokenizers {

StdRegex::StdRegex(const std::string& pattern) : regex_(pattern) {}

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

bool StdRegex::ok() const {
  // std::regex constructor throws if the pattern is invalid
  // If we got here, the pattern is valid
  return true;
}

} // namespace tokenizers
