#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include <espeak-ng/speak_lib.h>

#include "uni_algo.h"

#define CLAUSE_INTONATION_FULL_STOP 0x00000000
#define CLAUSE_INTONATION_COMMA 0x00001000
#define CLAUSE_INTONATION_QUESTION 0x00002000
#define CLAUSE_INTONATION_EXCLAMATION 0x00003000

#define CLAUSE_TYPE_CLAUSE 0x00040000
#define CLAUSE_TYPE_SENTENCE 0x00080000

#define CLAUSE_PERIOD (40 | CLAUSE_INTONATION_FULL_STOP | CLAUSE_TYPE_SENTENCE)
#define CLAUSE_COMMA (20 | CLAUSE_INTONATION_COMMA | CLAUSE_TYPE_CLAUSE)
#define CLAUSE_QUESTION (40 | CLAUSE_INTONATION_QUESTION | CLAUSE_TYPE_SENTENCE)
#define CLAUSE_EXCLAMATION                                                     \
  (45 | CLAUSE_INTONATION_EXCLAMATION | CLAUSE_TYPE_SENTENCE)
#define CLAUSE_COLON (30 | CLAUSE_INTONATION_FULL_STOP | CLAUSE_TYPE_CLAUSE)
#define CLAUSE_SEMICOLON (30 | CLAUSE_INTONATION_COMMA | CLAUSE_TYPE_CLAUSE)

using namespace std;

namespace piper {

typedef char32_t Phoneme;
typedef map<Phoneme, vector<Phoneme>> PhonemeMap;

struct eSpeakPhonemeConfig {
  string voice = "en-us";

  Phoneme period = U'.';      // CLAUSE_PERIOD
  Phoneme comma = U',';       // CLAUSE_COMMA
  Phoneme question = U'?';    // CLAUSE_QUESTION
  Phoneme exclamation = U'!'; // CLAUSE_EXCLAMATION
  Phoneme colon = U':';       // CLAUSE_COLON
  Phoneme semicolon = U';';   // CLAUSE_SEMICOLON

  // Remove language switch flags like "(en)"
  bool keepLanguageFlags = false;

  shared_ptr<PhonemeMap> phonemeMap;
};

// language -> phoneme -> [phoneme, ...]
map<string, PhonemeMap> DEFAULT_PHONEME_MAP = {{"pt-br", {{U'c', {U'k'}}}}};

// Phonemizes text using espeak-ng.
// Returns phonemes for each sentence as a separate vector.
//
// Assumes espeak_Initialize has already been called.
void phonemize_eSpeak(string text, eSpeakPhonemeConfig &config,
                      vector<vector<Phoneme>> &phonemes) {

  auto voice = config.voice;
  int result = espeak_SetVoiceByName(voice.c_str());
  if (result != 0) {
    throw runtime_error("Failed to set eSpeak-ng voice");
  }

  shared_ptr<PhonemeMap> phonemeMap;
  if (config.phonemeMap) {
    phonemeMap = config.phonemeMap;
  } else if (DEFAULT_PHONEME_MAP.count(voice) > 0) {
    phonemeMap = make_shared<PhonemeMap>(DEFAULT_PHONEME_MAP[voice]);
  }

  // Modified by eSpeak
  string textCopy(text);

  vector<Phoneme> *sentencePhonemes = nullptr;
  const char *inputTextPointer = textCopy.c_str();
  int terminator = 0;

  while (inputTextPointer != NULL) {
    // Modified espeak-ng API to get access to clause terminator
    string clausePhonemes(espeak_TextToPhonemesWithTerminator(
        (const void **)&inputTextPointer,
        /*textmode*/ espeakCHARS_AUTO,
        /*phonememode = IPA*/ 0x02, &terminator));

    // Decompose, e.g. "ç" -> "c" + "̧"
    auto phonemesNorm = una::norm::to_nfd_utf8(clausePhonemes);
    auto phonemesRange = una::ranges::utf8_view{phonemesNorm};

    if (!sentencePhonemes) {
      // Start new sentence
      phonemes.emplace_back();
      sentencePhonemes = &phonemes[phonemes.size() - 1];
    }

    // Maybe use phoneme map
    vector<Phoneme> mappedSentPhonemes;
    if (phonemeMap) {
      for (auto phoneme : phonemesRange) {
        if (phonemeMap->count(phoneme) < 1) {
          // No mapping for phoneme
          mappedSentPhonemes.push_back(phoneme);
        } else {
          // Mapping for phoneme
          auto mappedPhonemes = &(phonemeMap->at(phoneme));
          mappedSentPhonemes.insert(mappedSentPhonemes.end(),
                                    mappedPhonemes->begin(),
                                    mappedPhonemes->end());
        }
      }
    } else {
      // No phoneme map
      mappedSentPhonemes.insert(mappedSentPhonemes.end(), phonemesRange.begin(),
                                phonemesRange.end());
    }

    auto phonemeIter = mappedSentPhonemes.begin();
    auto phonemeEnd = mappedSentPhonemes.end();

    if (config.keepLanguageFlags) {
      // No phoneme filter
      sentencePhonemes->insert(sentencePhonemes->end(), phonemeIter,
                               phonemeEnd);
    } else {
      // Filter out (lang) switch (flags).
      // These surround words from languages other than the current voice.
      bool inLanguageFlag = false;

      while (phonemeIter != phonemeEnd) {
        if (inLanguageFlag) {
          if (*phonemeIter == U')') {
            // End of (lang) switch
            inLanguageFlag = false;
          }
        } else if (*phonemeIter == U'(') {
          // Start of (lang) switch
          inLanguageFlag = true;
        } else {
          sentencePhonemes->push_back(*phonemeIter);
        }

        phonemeIter++;
      }
    }

    // Add appropriate punctuation depending on terminator type
    int punctuation = terminator & 0x000FFFFF;
    if (punctuation == CLAUSE_PERIOD) {
      sentencePhonemes->push_back(config.period);
    } else if (punctuation == CLAUSE_QUESTION) {
      sentencePhonemes->push_back(config.question);
    } else if (punctuation == CLAUSE_EXCLAMATION) {
      sentencePhonemes->push_back(config.exclamation);
    } else if (punctuation == CLAUSE_COMMA) {
      sentencePhonemes->push_back(config.comma);
    } else if (punctuation == CLAUSE_COLON) {
      sentencePhonemes->push_back(config.colon);
    } else if (punctuation == CLAUSE_SEMICOLON) {
      sentencePhonemes->push_back(config.semicolon);
    }

    if ((terminator & CLAUSE_TYPE_SENTENCE) == CLAUSE_TYPE_SENTENCE) {
      // End of sentence
      sentencePhonemes = nullptr;
    }

  } // while inputTextPointer != NULL

} /* phonemize_eSpeak */

// ----------------------------------------------------------------------------

enum TextCasing { CASING_IGNORE, CASING_LOWER, CASING_UPPER, CASING_FOLD };

// Configuration for phonemize_codepoints
struct CodepointsPhonemeConfig {
  TextCasing casing = CASING_FOLD;
  shared_ptr<PhonemeMap> phonemeMap;
};

// "Phonemizes" text as a series of normalized UTF-8 codepoints.
// Returns a single vector of "phonemes".
//
// Does not detect sentence boundaries.
void phonemize_codepoints(string text, CodepointsPhonemeConfig &config,
                          vector<vector<Phoneme>> &phonemes) {

  if (config.casing == CASING_LOWER) {
    text = una::cases::to_lowercase_utf8(text);
  } else if (config.casing == CASING_UPPER) {
    text = una::cases::to_uppercase_utf8(text);
  } else if (config.casing == CASING_FOLD) {
    text = una::cases::to_casefold_utf8(text);
  }

  // Decompose, e.g. "ç" -> "c" + "̧"
  auto phonemesNorm = una::norm::to_nfd_utf8(text);
  auto phonemesRange = una::ranges::utf8_view{phonemesNorm};

  // No sentence boundary detection
  phonemes.emplace_back();
  auto sentPhonemes = &phonemes[phonemes.size() - 1];

  if (config.phonemeMap) {
    for (auto phoneme : phonemesRange) {
      if (config.phonemeMap->count(phoneme) < 1) {
        // No mapping for phoneme
        sentPhonemes->push_back(phoneme);
      } else {
        // Mapping for phoneme
        auto mappedPhonemes = &(config.phonemeMap->at(phoneme));
        sentPhonemes->insert(sentPhonemes->end(), mappedPhonemes->begin(),
                             mappedPhonemes->end());
      }
    }
  } else {
    // No phoneme map
    sentPhonemes->insert(sentPhonemes->end(), phonemesRange.begin(),
                         phonemesRange.end());
  }
} // phonemize_text

} // namespace piper
