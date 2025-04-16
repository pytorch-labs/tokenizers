#pragma once

#include <memory>
#include <string>

// Third Party
#include <re2/re2.h>

// Local
#include "regex.h"

/**
 * @brief RE2-based implementation of IRegex.
 */
class Re2Regex : public IRegex {
 public:
  /**
   * @brief Construct a RE2 regex with the given pattern.
   *
   * @param pattern The regex pattern to compile.
   */
  explicit Re2Regex(const std::string& pattern);

  /**
   * @brief Return all non-overlapping matches found in the input string.
   */
  virtual std::vector<Match> findAll(const std::string& text) const override;

  /**
   * @brief Check if RE2 compiled the pattern successfully.
   */
  bool ok() const override;

 protected:
  /**
   * @brief Expose internal RE2 pointer to the factory if needed.
   */
  const re2::RE2* rawRegex() const;

 private:
  std::unique_ptr<re2::RE2> regex_;

  friend tokenizers::Result<std::unique_ptr<IRegex>> createRegex(
      const std::string& pattern);
};
