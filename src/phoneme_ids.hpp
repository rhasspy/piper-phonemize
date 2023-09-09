#ifndef PHONEME_IDS_H_
#define PHONEME_IDS_H_

#include <map>
#include <string>
#include <vector>

#include "phonemize.hpp"
#include "shared.hpp"

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

extern PIPERPHONEMIZE_EXPORT const size_t MAX_PHONEMES;
extern PIPERPHONEMIZE_EXPORT PhonemeIdMap DEFAULT_PHONEME_ID_MAP;
extern PIPERPHONEMIZE_EXPORT std::map<std::string, PhonemeIdMap>
    DEFAULT_ALPHABET;

PIPERPHONEMIZE_EXPORT void
phonemes_to_ids(const std::vector<Phoneme> &phonemes, PhonemeIdConfig &config,
                std::vector<PhonemeId> &phonemeIds,
                std::map<Phoneme, std::size_t> &missingPhonemes);

} // namespace piper

#endif // PHONEME_IDS_H_
