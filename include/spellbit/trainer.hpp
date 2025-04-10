#ifndef SPELLBIT_TRAINER_HPP
#define SPELLBIT_TRAINER_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace spellbit {
using BytePair = std::pair<uint32_t, uint32_t>;
using FrequencyPair = std::pair<BytePair, uint32_t>;
struct PairFrequencyOrder {
  bool operator()(const FrequencyPair &a, const FrequencyPair &b) const {
    return a.second < b.second;
  }
};
struct BytePairHash {
  std::size_t operator()(const BytePair &p) const {
    return std::hash<uint32_t>{}(p.first) ^
           (std::hash<uint32_t>{}(p.second) << 1);
  }
};
class Trainer {
private:
  size_t max_vocab_size = 0;
  size_t n = 0;
  uint32_t current_id = 0;
  uint32_t vocab_id = 0;
  std::vector<uint32_t> create_vocab_entry(BytePair bp);
  void add_word(std::string word, std::vector<uint32_t> word_as_list);
  // Maybe std::list<uint32_t> would be better as key? We'd need to
  // hash though, so... future-me problem! Sorry!
  std::unordered_map<std::string, uint32_t> word_to_id;
  std::unordered_map<uint32_t, uint32_t> char_to_token;
  std::vector<uint32_t> word_count;
  std::vector<std::vector<uint32_t>> words;
  std::unordered_map<BytePair, uint32_t, BytePairHash> pair_frequencies;
  std::unordered_map<uint32_t, std::vector<BytePair>> word_id_to_pairs;
  std::unordered_map<BytePair, std::set<uint32_t>, BytePairHash> pair_to_word;
  std::vector<std::vector<uint32_t>> vocab;
  std::priority_queue<FrequencyPair, std::vector<FrequencyPair>,
                      PairFrequencyOrder>
      freq_pq;
  void initialize_queue_from_pairs();

public:
  Trainer(size_t max_vocab_size) : max_vocab_size(max_vocab_size) {};
  ~Trainer() {};

  void save_vocab(const std::string &filename);
  int add_file(std::string path);
  size_t train();
};

} // namespace spellbit
#endif
