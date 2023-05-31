#include <iostream>
#include <vector>

#include <espeak-ng/speak_lib.h>

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

  piper::eSpeakConfig config;
  config.voice = "de";
  vector<vector<piper::Phoneme>> phonemes;
  phonemize_eSpeak("licht", config, phonemes);

  string phonemeStr;
  for (auto sentPhonemes : phonemes) {
    phonemeStr = una::ranges::to_utf8<std::string>(sentPhonemes);
    std::cout << phonemeStr << std::endl;
  }

  espeak_Terminate();

  return 0;
}
