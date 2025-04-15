#pragma once

#include <memory>
#include <string>

// Define PCRE2 code unit width before including pcre2.h
#define PCRE2_CODE_UNIT_WIDTH 8

// Third Party
#include <pcre2.h>

// Local
#include "regex.h"

/**
 * @brief PCRE2-based implementation of IRegex.
 */
class Pcre2Regex : public IRegex {
 public:
  /**
   * @brief Construct a PCRE2 regex with the given pattern.
   *
   * @param pattern The regex pattern to compile.
   */
  explicit Pcre2Regex(const std::string& pattern);

  /**
   * @brief Destructor to clean up PCRE2 resources.
   */
  ~Pcre2Regex();

  /**
   * @brief Return all non-overlapping matches found in the input string.
   */
  virtual std::vector<Match> findAll(const std::string& text) const override;

  /**
   * @brief Check if PCRE2 compiled the pattern successfully.
   */
  bool ok() const;

 protected:
  /**
   * @brief Expose internal PCRE2 pointer to the factory if needed.
   */
  const pcre2_code* rawRegex() const;

 private:
  pcre2_code* regex_;
  pcre2_match_data* match_data_;
  bool is_valid_;

  friend std::unique_ptr<IRegex> createRegex(const std::string& pattern);
}; 