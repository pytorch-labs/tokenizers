# Copyright (c) Meta Platforms, Inc. and affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.
# @lint-ignore-every LICENSELINT


from enum import Enum
from typing import Optional

from .hf_tokenizer import HuggingFaceTokenizer
from .llama2c import Llama2cTokenizer
from .sentencepiece import SentencePieceTokenizer
from .tiktoken import TiktokenTokenizer


class TokenizerType(Enum):
    LLAMA2C = "llama2c"
    SENTENCEPIECE = "sentencepiece"
    TIKTOKEN = "tiktoken"
    HUGGINGFACE = "huggingface"
    
    @classmethod
    def from_str(cls, value: str) -> "TokenizerType":
        """Create TokenizerType from string value (case-insensitive)"""
        value_lower = value.lower()
        for tokenizer_type in cls:
            if tokenizer_type.value == value_lower:
                return tokenizer_type
        raise ValueError(f"Invalid tokenizer type: {value}. Valid options: {[t.value for t in cls]}")


__all__ = ["TiktokenTokenizer", "Llama2cTokenizer", "HuggingFaceTokenizer", "SentencePieceTokenizer", "TokenizerType"]


def get_tokenizer(
    tokenizer_path: str, 
    tokenizer_config_path: Optional[str] = None,
    tokenizer_type: Optional[TokenizerType] = None
):
    if tokenizer_type is not None:
        if tokenizer_type == TokenizerType.HUGGINGFACE:
            return HuggingFaceTokenizer(tokenizer_path, tokenizer_config_path)
        elif tokenizer_type == TokenizerType.LLAMA2C:
            return Llama2cTokenizer(model_path=str(tokenizer_path))
        elif tokenizer_type == TokenizerType.SENTENCEPIECE:
            return SentencePieceTokenizer(model_path=str(tokenizer_path))
        elif tokenizer_type == TokenizerType.TIKTOKEN:
            return TiktokenTokenizer(model_path=str(tokenizer_path))
    
    # Default fallback to auto-detection
    if tokenizer_path.endswith(".json"):
        tokenizer = HuggingFaceTokenizer(tokenizer_path, tokenizer_config_path)
    else:
        try:
            tokenizer = Llama2cTokenizer(model_path=str(tokenizer_path))
        except Exception:
            try:
                print("Using SentencePiece tokenizer")
                tokenizer = SentencePieceTokenizer(model_path=str(tokenizer_path))
            except Exception:
                print("Using Tiktokenizer")
                tokenizer = TiktokenTokenizer(model_path=str(tokenizer_path))
    return tokenizer
