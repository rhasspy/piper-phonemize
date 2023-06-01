#include <iostream>
#include <sstream>
#include <vector>

#include <espeak-ng/speak_lib.h>

#include "phoneme_ids.hpp"
#include "phonemize.hpp"

string idString(const vector<vector<piper::Phoneme>> &phonemes,
                piper::PhonemeIdConfig &idConfig) {
  std::stringstream idStr;
  for (auto sentPhonemes : phonemes) {
    vector<piper::PhonemeId> sentIds;
    piper::phonemes_to_ids(sentPhonemes, idConfig, sentIds);

    for (auto id : sentIds) {
      idStr << id << " ";
    }
  }

  return idStr.str();
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "Need espeak-ng-data path" << std::endl;
    return 1;
  }

  int result = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, argv[1], 0);
  if (result < 0) {
    std::cerr << "Failed to initialize eSpeak" << std::endl;
    return 1;
  }

  piper::eSpeakPhonemeConfig phonemeConfig;
  piper::PhonemeIdConfig idConfig;

  // Check "licht" in German
  phonemeConfig.voice = "de";
  vector<vector<piper::Phoneme>> phonemes;

  // Should be "lˈɪçt!" where "ç" is decomposed into two codepoints
  piper::phonemize_eSpeak("licht!", phonemeConfig, phonemes);

  // 0 = pad
  // 1 = bos
  // 2 = eos
  // 4 = !
  string idStr = idString(phonemes, idConfig);
  if (idStr != "1 0 24 0 120 0 74 0 16 0 140 0 32 0 4 0 2 ") {
    std::cerr << "licht: " << idStr << std::endl;
    return 1;
  }

  // Check "ВЕСЕ́ЛКА" in Ukrainian
  piper::CodepointsPhonemeConfig codepointsConfig;
  phonemes.clear();

  piper::phonemize_codepoints("ВЕСЕ́ЛКА", textConfig, phonemes);

  // Case folded and NFD normalized
  vector<piper::Phoneme> expectedPhonemes = {U'в', U'е', U'с', U'е',
                                             U'́',  U'л', U'к', U'а'};

  if (phonemes[0] != expectedPhonemes) {
    std::cerr << "Весе́лка: ";
    for (auto phoneme : phonemes[0]) {
      std::cerr << phoneme << " ";
    }

    std::cerr << std::endl;
    return 1;
  }

  idConfig.phonemeIdMap =
      make_shared<piper::PhonemeIdMap>(piper::DEFAULT_ALPHABET["uk"]);
  idStr = idString(phonemes, idConfig);
  if (idStr != "1 0 14 0 18 0 33 0 18 0 45 0 27 0 26 0 12 0 2 ") {
    std::cerr << "Весе́лка: " << idStr << std::endl;
    return 1;
  }

  // --------------------------------------------------------------------------

  std::cout << "OK" << std::endl;

  espeak_Terminate();

  return 0;
}
