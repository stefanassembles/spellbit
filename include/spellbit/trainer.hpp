#ifndef SPELLBIT_TRAINER_HPP
#define SPELLBIT_TRAINER_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace spellbit {
class Trainer {
private:
  size_t n = 0;
  uint32_t current_id = 0;
  int8_t get_utf_char_size(unsigned char byte);
  void add_word(std::string word);
  std::unordered_map<std::string, uint32_t> word_to_id;
  std::vector<uint32_t> word_count;
  std::vector<std::string> words;
  std::unordered_map<std::string, uint32_t> pair_frequencies;
  std::unordered_map<std::string, uint32_t> vocab;
  std::vector<std::pair<std::string, std::string>> merges;

public:
  Trainer();
  ~Trainer();

  int addFile(std::string path);
  int train(); 
};

}
#endif

