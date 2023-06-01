from pathlib import Path
from typing import List, Optional, Union

from piper_phonemize_cpp import (
    phonemize_espeak as _phonemize_espeak,
    phonemize_codepoints,
    phoneme_ids_espeak,
    phoneme_ids_codepoints,
)

_DIR = Path(__file__).parent


def phonemize_espeak(
    text: str, voice: str, data_path: Optional[Union[str, Path]] = None
) -> List[List[str]]:
    if data_path is None:
        data_path = _DIR / "espeak-ng-data"

    return _phonemize_espeak(text, voice, str(data_path))
