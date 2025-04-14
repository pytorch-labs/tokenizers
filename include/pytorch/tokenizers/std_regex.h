#pragma once

#include <memory>
#include <regex>
#include <string>
#include "regex.h"

/**
 * @brief std::regex-based implementation of IRegex.
 */
class StdRegex : public IRegex {
 public:
  /**
   * @brief Construct a std::regex wrapper with the given pattern.
   *
   * @param pattern The regex pattern to compile.
   * @throws std::regex_error if the pattern is invalid.
   */
  explicit StdRegex(const std::string& pattern);

  /**
   * @brief Find all non-overlapping matches in the input string.
   */
  virtual std::vector<Match> findAll(const std::string& text) const override;

 private:
  std::regex regex_;
};
