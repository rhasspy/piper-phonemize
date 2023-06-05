from collections import Counter
from enum import Enum
from pathlib import Path
from typing import Dict, List, Optional, Union

from piper_phonemize_cpp import (
    phonemize_espeak as _phonemize_espeak,
    phonemize_codepoints as _phonemize_codepoints,
    phoneme_ids_espeak as _phonemize_ids_espeak,
    phoneme_ids_codepoints as _phonemize_ids_codepoints,
    get_espeak_map,
    get_codepoints_map,
    get_max_phonemes,
)

_DIR = Path(__file__).parent


class TextCasing(str, Enum):
    """Casing applied to text for phonemize_codepoints"""

    IGNORE = "ignore"
    LOWER = "lower"
    UPPER = "upper"
    FOLD = "fold"


def phonemize_espeak(
    text: str,
    voice: str,
    data_path: Optional[Union[str, Path]] = None,
) -> List[List[str]]:
    if data_path is None:
        data_path = _DIR / "espeak-ng-data"

    return _phonemize_espeak(text, voice, str(data_path))


def phonemize_codepoints(
    text: str,
    casing: Union[str, TextCasing] = TextCasing.FOLD,
) -> List[List[str]]:
    casing = TextCasing(casing)
    return _phonemize_codepoints(text, casing.value)


def phoneme_ids_espeak(
    phonemes: List[str],
    missing_phonemes: "Optional[Counter[str]]" = None,
) -> List[int]:
    phoneme_ids, missing_counts = _phonemize_ids_espeak(phonemes)
    if missing_phonemes is not None:
        missing_phonemes.update(missing_counts)

    return phoneme_ids


def phoneme_ids_codepoints(
    language: str,
    phonemes: List[str],
    missing_phonemes: "Optional[Counter[str]]" = None,
) -> List[int]:
    phoneme_ids, missing_counts = _phonemize_ids_codepoints(language, phonemes)
    if missing_phonemes is not None:
        missing_phonemes.update(missing_counts)

    return phoneme_ids
