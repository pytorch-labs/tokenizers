/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Adopted from https://github.com/sewenew/tokenizer

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

#include <pytorch/tokenizers/base64.h>
#include <pytorch/tokenizers/tiktoken.h>
#include <algorithm>
#include <cinttypes>
#include <fstream>
#include <limits>
#include <unordered_set>
#include "re2/re2.h"

namespace tokenizers {

using namespace detail;

// ------------------------------Util start------------------------------------
namespace {

static Re2UPtr _create_regex(const std::string& pattern) {
  assert(!pattern.empty());

  return std::make_unique<re2::RE2>("(" + pattern + ")");
}

static Re2UPtr _build_special_token_regex(
    const std::vector<std::pair<std::string, std::uint64_t>>& special_encoder) {
  std::string special_pattern;
  for (const auto& ele : special_encoder) {
    if (!special_pattern.empty()) {
      special_pattern += "|";
    }
    special_pattern += re2::RE2::QuoteMeta(ele.first);
  }

  if (special_pattern.empty()) {
    return nullptr;
  }

  return _create_regex(special_pattern);
}

static Result<std::pair<std::string, uint64_t>> _parse(
    const std::string& line) {
  // Tiktoken format
  // https://github.com/openai/tiktoken/blob/main/tiktoken/load.py#L140 <base64
  // encoded token str> <rank>
  auto pos = line.find(" ");
  TK_CHECK_OR_RETURN_ERROR(
      pos != std::string::npos,
      ParseFailure,
      "invalid tiktoken line: %s",
      line.c_str());

  auto token = TK_UNWRAP(base64::decode({line.data(), pos}));
  uint64_t rank = 0;
  try {
    rank = std::stoul(line.substr(pos + 1));
  } catch (const std::exception&) {
    TK_CHECK_OR_RETURN_ERROR(
        false, EncodeFailure, "invalid encoder rank: %s", line.c_str());
  }

  return std::pair{std::move(token), rank};
}

static Result<TokenMap> _load_token_map(const std::string& path) {
  std::ifstream file(path);
  TK_CHECK_OR_RETURN_ERROR(
      file, LoadFailure, "failed to open encoder file: %s", path.c_str());

  // Instead of generating couple of large unordered_maps here to only process
  // them linearly in the TokenMap, just place them in a vector of pairs and
  // sort them twice, looking for duplicates.  It's still O(n log n) but avoids
  // the overhead of the unordered_maps.

  std::vector<std::pair<std::string, uint64_t>> pairs;
  std::string line;
  while (std::getline(file, line)) {
    auto [token, rank] = TK_UNWRAP(_parse(line));
    pairs.emplace_back(std::move(token), rank);
  }

  return buildTokenMap(pairs);
}

} // namespace

// ------------------------------Util end------------------------------------
// -------------------------private method start-------------------------------

Error Tiktoken::_encode(
    re2::StringPiece& input,
    std::vector<uint64_t>& ret,
    uint64_t& last_piece_token_len) const {
  std::string piece;
  assert(_regex);
  while (re2::RE2::FindAndConsume(&input, *_regex, &piece)) {
    const auto result = token_map_->tryGetInteger(piece);
    if (result) {
      last_piece_token_len = 1;
      ret.push_back(*result);
      continue;
    }
    auto tokens = TK_UNWRAP(byte_pair_encode_(piece, *token_map_));
    last_piece_token_len = tokens.size();
    ret.insert(ret.end(), tokens.begin(), tokens.end());
  }
  return Error::Ok;
}

void Tiktoken::_decode(re2::StringPiece input, std::string& ret) const {
#ifdef _USE_INTERNAL_STRING_VIEW
  ret += input.as_string();
#else
  ret += input;
#endif
}

// -------------------------private method end-------------------------------
// -------------------------public method start-------------------------------

Error Tiktoken::load(const std::string& path) {
  token_map_.emplace(TK_UNWRAP(_load_token_map(path)));

  std::vector<std::pair<std::string, uint64_t>> special_token_map;
  for (std::size_t i = 0; i < _special_tokens->size(); ++i) {
    special_token_map.emplace_back(
        _special_tokens->at(i), token_map_->size() + i);
  }

  special_token_map_.emplace(TokenMap(special_token_map));

  _regex = _create_regex(_pattern);
  // Warmup re2 as it is slow on the first run, void the return value as it's
  // not needed Refer to
  // https://github.com/google/re2/blob/6dcd83d60f7944926bfd308cc13979fc53dd69ca/re2/fuzzing/re2_fuzzer.cc#L136-L141
  (void)_regex->ReverseProgramSize();

  special_token_regex_ = _build_special_token_regex(special_token_map);
  // Same as above, warm up re2
  (void)special_token_regex_->ReverseProgramSize();

  // initialize vocab_size, bos_tok, eos_tok
  vocab_size_ = token_map_->size() + special_token_map_->size();
  bos_tok_ =
      *special_token_map_->tryGetInteger(_special_tokens->at(_bos_token_index));
  eos_tok_ =
      *special_token_map_->tryGetInteger(_special_tokens->at(_eos_token_index));

  initialized_ = true;
  return Error::Ok;
}

// -------------------------public method end-------------------------------

} // namespace tokenizers
