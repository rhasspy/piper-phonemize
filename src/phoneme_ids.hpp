#ifndef PHONEME_IDS_H_
#define PHONEME_IDS_H_

#include <map>
#include <string>
#include <vector>

#include "phonemize.hpp"

// Basically cmake export all symbols in windows still requires
// explicit export of data symbols
#if defined(_WIN32)
  #ifdef piper_phonemize_EXPORTS
    #define API_DATA __declspec(dllexport)
  #else
    #define API_DATA __declspec(dllimport)
  #endif
#else
  #define API
#endif

namespace piper {

typedef int64_t PhonemeId;
typedef std::map<Phoneme, std::vector<PhonemeId>> PhonemeIdMap;

struct PhonemeIdConfig {
  Phoneme pad = U'_';
  Phoneme bos = U'^';
  Phoneme eos = U'$';

  // Every other phoneme id is pad
  bool interspersePad = true;

  // Add beginning of sentence (bos) symbol at start
  bool addBos = true;

  // Add end of sentence (eos) symbol at end
  bool addEos = true;

  // Map from phonemes to phoneme id(s).
  // Not set means to use DEFAULT_PHONEME_ID_MAP.
  std::shared_ptr<PhonemeIdMap> phonemeIdMap;
};

extern API_DATA const size_t MAX_PHONEMES;
extern API_DATA PhonemeIdMap DEFAULT_PHONEME_ID_MAP;
extern API_DATA std::map<std::string, PhonemeIdMap> DEFAULT_ALPHABET;

void phonemes_to_ids(const std::vector<Phoneme> &phonemes,
                     PhonemeIdConfig &config,
                     std::vector<PhonemeId> &phonemeIds,
                     std::map<Phoneme, std::size_t> &missingPhonemes);

} // namespace piper

#endif // PHONEME_IDS_H_
