/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
// @lint-ignore-every LICENSELINT
/**************************************************************************
   Copyright (c) 2023 sewenew

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *************************************************************************/

#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include <string_view>

#include "result.h"

namespace base64 {

using tokenizers::Error;
using tokenizers::Result;

Result<std::string> decode(const std::string_view& input);

namespace detail {

constexpr uint32_t DECODE_TABLE[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62,  255,
    255, 255, 63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255,
    255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  255, 255, 255, 255, 255, 255, 26,  27,  28,  29,  30,  31,  32,  33,
    34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255};

inline Error validate(uint32_t v) {
  if (v == 255) {
    fprintf(stderr, "invalid char");
    return Error::Base64DecodeFailure;
  }
  return Error::Ok;
}

inline Error decode(const std::string_view& input, std::string& output) {
  TK_CHECK_OR_RETURN_ERROR(
      input.size() == 4,
      Base64DecodeFailure,
      "input length must be 4, got %zu",
      input.size());

  uint32_t val = 0;

  uint8_t c = input[0];
  auto v = DECODE_TABLE[c];
  TK_CHECK_OK_OR_RETURN_ERROR(validate(v));
  val = v;

  c = input[1];
  v = DECODE_TABLE[c];
  TK_CHECK_OK_OR_RETURN_ERROR(validate(v));
  val = (val << 6) | v;

  c = input[2];
  v = DECODE_TABLE[c];
  TK_CHECK_OK_OR_RETURN_ERROR(validate(v));
  val = (val << 6) | v;

  c = input[3];
  v = DECODE_TABLE[c];
  TK_CHECK_OK_OR_RETURN_ERROR(validate(v));
  val = (val << 6) | v;

  output.push_back(static_cast<char>((val >> 16) & 0xFF));
  output.push_back(static_cast<char>((val >> 8) & 0xFF));
  output.push_back(static_cast<char>(val & 0xFF));
  return Error::Ok;
}

inline Error decode_1_padding(
    const std::string_view& input,
    std::string& output) {
  TK_CHECK_OR_RETURN_ERROR(
      input.size() == 3,
      Base64DecodeFailure,
      "input length must be 3, got %zu",
      input.size());

  uint32_t val = 0;

  uint8_t c = input[0];
  auto v = DECODE_TABLE[c];
  TK_CHECK_OK_OR_RETURN_ERROR(validate(v));
  val = v;

  c = input[1];
  v = DECODE_TABLE[c];
  TK_CHECK_OK_OR_RETURN_ERROR(validate(v));
  val = (val << 6) | v;

  c = input[2];
  v = DECODE_TABLE[c];
  TK_CHECK_OK_OR_RETURN_ERROR(validate(v));
  val = (val << 6) | v;

  output.push_back(static_cast<char>((val >> 10) & 0xFF));
  output.push_back(static_cast<char>((val >> 2) & 0xFF));
  return Error::Ok;
}

inline Error decode_2_padding(
    const std::string_view& input,
    std::string& output) {
  TK_CHECK_OR_RETURN_ERROR(
      input.size() == 2,
      Base64DecodeFailure,
      "input length must be 2, got %zu",
      input.size());

  uint32_t val = 0;

  uint8_t c = input[0];
  auto v = DECODE_TABLE[c];
  TK_CHECK_OK_OR_RETURN_ERROR(validate(v));
  val = v;

  c = input[1];
  v = DECODE_TABLE[c];
  TK_CHECK_OK_OR_RETURN_ERROR(validate(v));
  val = (val << 6) | v;

  output.push_back(static_cast<char>((val >> 4) & 0xFF));
  return Error::Ok;
}

} // namespace detail

inline tokenizers::Result<std::string> decode(const std::string_view& input) {
  TK_CHECK_OR_RETURN_ERROR(!input.empty(), Base64DecodeFailure, "empty input");

  // Faster than `input.size() % 4`.
  TK_CHECK_OR_RETURN_ERROR(
      (input.size() & 3) == 0 && input.size() >= 4,
      Base64DecodeFailure,
      "input length must be larger than 4 and is multiple of 4, got %zu",
      input.size());

  std::string output;
  output.reserve(input.size() / 4 * 3);
  auto idx = 0U;
  for (; idx < input.size() - 4; idx += 4) {
    TK_CHECK_OK_OR_RETURN_ERROR(detail::decode(input.substr(idx, 4), output));
  }

  // Last 4 bytes. Might contain paddings.
  if (input[idx + 3] == '=') {
    if (input[idx + 2] == '=') {
      // Tow paddings.
      TK_CHECK_OK_OR_RETURN_ERROR(
          detail::decode_2_padding(input.substr(idx, 2), output));
    } else {
      // One padding.
      TK_CHECK_OK_OR_RETURN_ERROR(
          detail::decode_1_padding(input.substr(idx, 3), output));
    }
  } else {
    // No padding.
    TK_CHECK_OK_OR_RETURN_ERROR(detail::decode(input.substr(idx, 4), output));
  }

  return output;
}
} // namespace base64
