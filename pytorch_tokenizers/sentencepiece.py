# Copyright (c) Meta Platforms, Inc. and affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.
# @lint-ignore-every LICENSELINT

import logging
import os
from typing import List

from sentencepiece import SentencePieceProcessor


class SentencePieceTokenizer:
    def __init__(self, model_path: str):
        assert os.path.isfile(
            model_path
        ), f"Need a valid tokenizer model path but got {model_path}"
        self.sp_model = SentencePieceProcessor(model_file=model_path)
        self.model_path = model_path

        self.n_words: int = self.sp_model.vocab_size()
        self.bos_id: int = self.sp_model.bos_id()
        self.eos_id: int = self.sp_model.eos_id()
        logging.info(
            f"SentencePiece - #words: {self.n_words} - BOS ID: {self.bos_id} - EOS ID: {self.eos_id}"
        )

    def encode(self, s: str, bos: bool = False, eos: bool = False) -> List[int]:
        assert type(s) is str
        t = self.sp_model.encode(s)
        if bos:
            t = [self.bos_id] + t
        if eos:
            t = t + [self.eos_id]
        return t

    def decode(self, t: List[int]) -> str:
        return self.sp_model.decode(t)

    def decode_token(self, t: int) -> str:
        return self.sp_model.id_to_piece(t)
