#include <iostream>
#include <vector>

#include <espeak-ng/speak_lib.h>

#include "phoneme_ids.hpp"
#include "phonemize.hpp"

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

  piper::eSpeakConfig phonemeConfig;
  phonemeConfig.voice = "de";
  vector<vector<piper::Phoneme>> phonemes;
  phonemize_eSpeak("licht", phonemeConfig, phonemes);

  piper::PhonemeIdConfig idConfig;

  string phonemeStr;
  for (auto sentPhonemes : phonemes) {
    vector<piper::PhonemeId> sentIds;
    phonemes_to_ids(sentPhonemes, idConfig, sentIds);

    for (auto id : sentIds) {
      std::cout << id << " ";
    }

    std::cout << std::endl;
  }

  espeak_Terminate();

  return 0;
}
