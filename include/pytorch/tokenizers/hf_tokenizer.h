/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Used by many Huggingface models. Adapted from a combination of the original
// rust implementation (https://github.com/huggingface/tokenizers/tree/main)
// and the corresponding support in llama.cpp
// (https://github.com/ggerganov/llama.cpp)
#pragma once

// Standard
#include <string>

// Local
#include <pytorch/tokenizers/bpe_tokenizer_base.h>
#include <pytorch/tokenizers/error.h>
#include <pytorch/tokenizers/pre_tokenizer.h>
#include <pytorch/tokenizers/result.h>
#include <pytorch/tokenizers/token_decoder.h>

namespace tokenizers {

// Simple Word structure to mimic Rust's Word behavior
struct HFWord {
  std::vector<uint64_t> tokens;
  std::vector<size_t> byte_lengths;

  void add(uint64_t token_id, size_t byte_len) {
    tokens.push_back(token_id);
    byte_lengths.push_back(byte_len);
  }

  size_t size() const {
    return tokens.size();
  }

  // Apply all possible merges using the merge ranks
  void merge_all(
      const detail::TokenMap& merge_ranks,
      const detail::TokenMap& token_map) {
    while (tokens.size() > 1) {
      std::optional<std::pair<size_t, uint32_t>> best_merge;

      // Find the best merge (lowest rank) among adjacent token pairs
      for (size_t i = 0; i < tokens.size() - 1; ++i) {
        // Create the merged token string to look up its rank
        auto first_token = token_map.tryGetString(tokens[i]);
        auto second_token = token_map.tryGetString(tokens[i + 1]);

        if (first_token && second_token) {
          std::string merged_token =
              std::string(*first_token) + std::string(*second_token);
          auto rank = merge_ranks.tryGetInteger(merged_token);

          if (rank && (!best_merge || *rank < best_merge->second)) {
            best_merge = std::make_pair(i, static_cast<uint32_t>(*rank));
          }
        }
      }

      if (!best_merge) {
        break; // No more merges possible
      }

      // Apply the best merge
      size_t merge_idx = best_merge->first;

      // Get the merged token ID
      auto first_token = token_map.tryGetString(tokens[merge_idx]);
      auto second_token = token_map.tryGetString(tokens[merge_idx + 1]);

      if (first_token && second_token) {
        std::string merged_token =
            std::string(*first_token) + std::string(*second_token);
        auto merged_id = token_map.tryGetInteger(merged_token);

        if (merged_id) {
          // Replace the two tokens with the merged token
          tokens[merge_idx] = *merged_id;
          byte_lengths[merge_idx] += byte_lengths[merge_idx + 1];

          // Remove the second token
          tokens.erase(tokens.begin() + merge_idx + 1);
          byte_lengths.erase(byte_lengths.begin() + merge_idx + 1);
        } else {
          break; // Merged token not found in vocabulary
        }
      } else {
        break; // Original tokens not found in vocabulary
      }
    }
  }
};

class HFTokenizer : public detail::BPETokenizerBase {
 public:
  /*-- Public Interface --*/

  /**
   * Default initialize with no loaded data
   */
  explicit HFTokenizer() {}
  ~HFTokenizer() {}

  /**
   * Load the model data into the
   */
  Error load(const std::string& tokenizer_path) override;

 private:
  Error _encode(
      const std::string& input,
      std::vector<uint64_t>& ret,
      uint64_t& last_piece_token_len) const override;

  void _decode(const std::string& input, std::string& ret) const override;

  Result<std::vector<uint64_t>> byte_pair_encode_(
      const std::string& piece,
      const detail::TokenMap& encoder) const override;

  // Override the virtual _byte_pair_merge method to use HF-specific BPE logic
  std::vector<uint64_t> _byte_pair_merge(
      const std::string& piece,
      const detail::TokenMap& ranks,
      std::function<uint64_t(uint64_t, uint64_t)> func) const override;

  PreTokenizer::Ptr _pretokenizer;
  TokenDecoder::Ptr _decoder;
};

} // namespace tokenizers
