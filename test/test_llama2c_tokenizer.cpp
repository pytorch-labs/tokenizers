/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <pytorch/tokenizers/llama2c_tokenizer.h>

using namespace ::testing;

namespace tokenizers {

class Llama2cTokenizerTest : public Test {
public:
  void SetUp() override {
    tokenizer_ = std::make_unique<Llama2cTokenizer>();
    modelPath_ = std::getenv("RESOURCES_PATH") +
                 std::string("/test_llama2c_tokenizer.bin");
  }

  std::unique_ptr<Tokenizer> tokenizer_;
  std::string modelPath_;
};

TEST_F(Llama2cTokenizerTest, EncodeWithoutLoadFails) {
  Result<std::vector<uint64_t>> res = tokenizer_->encode("hello world", 0, 0);
  EXPECT_EQ(res.error(), Error::Uninitialized);
}

TEST_F(Llama2cTokenizerTest, DecodeWithoutLoadFails) {
  auto result = tokenizer_->decode(0, 0);
  EXPECT_EQ(result.error(), Error::Uninitialized);
}

TEST_F(Llama2cTokenizerTest, DecodeOutOfRangeFails) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);
  auto result = tokenizer_->decode(0, 64000);
  // The vocab size is 32000, and token 64000 is out of vocab range.
  EXPECT_EQ(result.error(), Error::OutOfRange);
}

TEST_F(Llama2cTokenizerTest, TokenizerMetadataIsExpected) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);
  // test_bpe_tokenizer.bin has vocab_size 0, bos_id 0, eos_id 0 recorded.
  EXPECT_EQ(tokenizer_->vocab_size(), 0);
  EXPECT_EQ(tokenizer_->bos_tok(), 0);
  EXPECT_EQ(tokenizer_->eos_tok(), 0);
}

TEST_F(Llama2cTokenizerTest, SafeToDestruct) {
  // Safe to destruct initialized tokenizer.
  tokenizer_->load(modelPath_);
  tokenizer_.reset();

  // Safe to destruct uninitialized tokenizer.
  tokenizer_ = std::make_unique<Llama2cTokenizer>();
  tokenizer_.reset();
}

} // namespace tokenizers
