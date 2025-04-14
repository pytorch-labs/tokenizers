#pragma once

#include <memory>
#include <string>
#include <vector>

struct Match {
  std::string text;
  size_t position;
};

/**
 * @brief Abstract interface for regex wrappers.
 */
class IRegex {
 public:
  virtual ~IRegex() = default;

  /**
   * @brief Find all non-overlapping matches in the input string.
   *
   * @param text The input string to search.
   * @return A vector of strings containing all matched substrings.
   */
  virtual std::vector<Match> findAll(const std::string& text) const = 0;
};

/**
 * @brief Creates a regex instance. Tries RE2 first, falls back to std::regex.
 *
 * @param pattern The regex pattern to compile.
 * @return A unique pointer to an IRegex-compatible object.
 */
std::unique_ptr<IRegex> createRegex(const std::string& pattern);
