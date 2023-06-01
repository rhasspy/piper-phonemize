#include <iostream>
#include <string>
#include <vector>

#include <espeak-ng/speak_lib.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "phoneme_ids.hpp"
#include "phonemize.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

// True when espeak_Initialize has been called
bool eSpeakInitialized = false;

std::vector<std::vector<piper::Phoneme>>
phonemize_espeak(std::string text, std::string voice, std::string dataPath) {
  if (!eSpeakInitialized) {
    int result =
        espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, dataPath.c_str(), 0);
    if (result < 0) {
      throw std::runtime_error("Failed to initialize eSpeak");
    }

    eSpeakInitialized = true;
  }

  piper::eSpeakPhonemeConfig config;
  config.voice = voice;

  std::vector<std::vector<piper::Phoneme>> phonemes;
  piper::phonemize_eSpeak(text, config, phonemes);

  return phonemes;
}

std::vector<std::vector<piper::Phoneme>>
phonemize_codepoints(std::string text) {
  piper::CodepointsPhonemeConfig config;
  std::vector<std::vector<piper::Phoneme>> phonemes;

  piper::phonemize_codepoints(text, config, phonemes);

  return phonemes;
}

std::vector<piper::PhonemeId>
phoneme_ids_espeak(std::vector<piper::Phoneme> phonemes) {
  piper::PhonemeIdConfig config;
  std::vector<piper::PhonemeId> phonemeIds;

  phonemes_to_ids(phonemes, config, phonemeIds);

  return phonemeIds;
}

std::vector<piper::PhonemeId>
phoneme_ids_codepoints(std::string language,
                       std::vector<piper::Phoneme> phonemes) {
  if (piper::DEFAULT_ALPHABET.count(language) < 1) {
    throw std::runtime_error("No phoneme/id map for language");
  }

  piper::PhonemeIdConfig config;
  config.phonemeIdMap =
      std::make_shared<piper::PhonemeIdMap>(piper::DEFAULT_ALPHABET[language]);
  std::vector<piper::PhonemeId> phonemeIds;

  phonemes_to_ids(phonemes, config, phonemeIds);

  return phonemeIds;
}

PYBIND11_MODULE(piper_phonemize_cpp, m) {
  m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: piper_phonemize_cpp

        .. autosummary::
           :toctree: _generate

           phonemize_espeak
           phonemize_codepoints
           phoneme_ids_espeak
           phoneme_ids_codepoints
    )pbdoc";

  m.def("phonemize_espeak", &phonemize_espeak, R"pbdoc(
        Phonemize text using espeak-ng
    )pbdoc");

  m.def("phonemize_codepoints", &phonemize_codepoints, R"pbdoc(
        Phonemize text as UTF-8 codepoints
    )pbdoc");

  m.def("phoneme_ids_espeak", &phoneme_ids_espeak, R"pbdoc(
        Get ids for espeak-ng phonemes
    )pbdoc");

  m.def("phoneme_ids_codepoints", &phoneme_ids_codepoints, R"pbdoc(
        Get ids for a language's codepoints
    )pbdoc");

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}
