# Copyright (c) Meta Platforms, Inc. and affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.
# @lint-ignore-every LICENSELINT

"""
Test script for hf tokenizers.
"""

import os
import sys
import unittest
from pytorch_tokenizers import CppHFTokenizer
from transformers import AutoTokenizer
from tempfile import TemporaryDirectory

PROMPT = "What is the capital of France?"

class TestHfTokenizer(unittest.TestCase):
    def setUp(self) -> None:
        self.temp_dir = TemporaryDirectory()
        super().setUp()

    def test_smolLM3(self) -> None:
        tokenizer = AutoTokenizer.from_pretrained("HuggingFaceTB/SmolLM3-3B")
        tokenizer_path = tokenizer.save_pretrained(self.temp_dir.name)[-1]

        cpp_tokenizer = CppHFTokenizer()
        cpp_tokenizer.load(tokenizer_path)

        tokens = tokenizer.encode(PROMPT)
        cpp_tokens = cpp_tokenizer.encode(PROMPT)
        self.assertEqual(tokens, cpp_tokens)

    def test_llama3_2_1b(self) -> None:
        tokenizer = AutoTokenizer.from_pretrained("unsloth/Llama-3.2-1B-Instruct")
        tokenizer_path = tokenizer.save_pretrained(self.temp_dir.name)[-1]

        cpp_tokenizer = CppHFTokenizer()
        cpp_tokenizer.load(tokenizer_path)

        tokens = tokenizer.encode(PROMPT)
        cpp_tokens = cpp_tokenizer.encode(PROMPT, bos=1)
        self.assertEqual(tokens, cpp_tokens)
        

    async def test_async_DO_NOT_COMMIT(self) -> None:
        pass
