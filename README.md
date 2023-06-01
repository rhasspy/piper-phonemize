# Piper Phonemization Library

Converts text to phonemes for [Piper](https://github.com/rhasspy/piper).

Requires [espeak-ng fork](https://github.com/rhasspy/espeak-ng) with `espeak_TextToPhonemesWithTerminator` function (allows punctuation preservation).


## Usage

See `src/test.cpp` for a C++ example.

### Python

The `piper_phonemize` Python package is built using [pybind11](https://pybind11.readthedocs.io).


## Building

Use Docker:

``` sh
docker buildx build . -t piper-phonemize --output 'type=local,dest=dist'
```

Find library and Python wheels in `dist/`

