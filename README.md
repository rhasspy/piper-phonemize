# Piper Phonemization Library

Converts text to phonemes for [Piper](https://github.com/rhasspy/piper).

When using eSpeak phonemes, requires an [espeak-ng fork](https://github.com/rhasspy/espeak-ng) with `espeak_TextToPhonemesWithTerminator` function.
This function allows for Piper to preserve punctuation and detect sentence boundaries.


## Usage

See `src/test.cpp` for a C++ example.

### Python

The `piper_phonemize` Python package is built using [pybind11](https://pybind11.readthedocs.io).

See `src/python_test.py` for a Python example.


## Building

Use Docker:

``` sh
docker buildx build . -t piper-phonemize --output 'type=local,dest=dist'
```

Find library and Python wheels in `dist/`

