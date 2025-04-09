#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <spellbit/trainer.hpp>
#include <utility>
#include <vector>
#include <spellbit/helpers.hpp>
#include <spellbit/count_min_sketch.hpp>

namespace spellbit {
void Trainer::add_word(std::string word_as_chars,
                       std::vector<uint32_t> word_as_tokens) {

  if (word_to_id.find(word_as_chars) == word_to_id.end()) {
    uint32_t new_id = words.size();
    word_to_id[word_as_chars] = new_id;
    words.push_back(word_as_tokens);
    if (new_id >= word_count.size()) {
      word_count.resize(new_id + 1, 0); // Initialize new slots with 0
    }
    if (word_as_tokens.size() >= 2) {
      word_count[new_id] = 1;
      auto it = word_as_tokens.begin();
      auto nextIt = std::next(it);

      do {
        BytePair pair = std::make_pair(*it, *nextIt);
        pair_to_word[pair].insert(new_id);
        word_id_to_pairs[new_id].push_back(pair);
        ++it;
        ++nextIt;
      } while (nextIt != word_as_tokens.end());
    }
    static size_t count_it = 0;
    if (++count_it % 1000000 == 0) {
      std::cout << "Added " << count_it << " words" << std::endl;
    }
  }
  uint32_t word_id = word_to_id[word_as_chars];

  word_count[word_id]++;

  for (const auto& pair : word_id_to_pairs[word_id]) {
    pair_frequencies[pair]++;
  }

}

int Trainer::add_file(std::string path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    std::cerr << "Unable to open file: " << path << "\n";
    return -1;
  }

  const size_t BUFFER_SIZE = 4096;
  char buffer[BUFFER_SIZE];
  char character[4];
  int8_t c_index = -1;
  int8_t max_size = 0;
  int charsRead = 0;
  int chunkSize;
  std::string word;
  std::vector<uint32_t> word_as_list;
  uint32_t last_char = 0;
  CountMinSketch cms(5, 200);
  auto processCharacter = [&](char c) {
    int8_t c_size = get_utf_char_size(c);


    if (c_index == -1) {
      if (c_size <= 0) { // Handle error cases together
        std::cerr << "Malformed or unexpected UTF-8 byte: "
                  << static_cast<int>(c) << "\n";
        c_index = -1;
        return;
      }
      c_index = 0;
      max_size = c_size;
    }
    character[c_index++] = c;


    if (c_index == max_size) {
      uint32_t encoded_char = utf8_to_uint32(character, max_size);
      std::string char_s = std::string(character, max_size);

      if (char_to_token.find(encoded_char) == char_to_token.end()) {
        char_to_token[encoded_char] = vocab_id++;
      }

      last_char = encoded_char;

      if ((encoded_char < 128) &&
     (std::isspace(static_cast<unsigned char>(encoded_char)) ||
      std::ispunct(static_cast<unsigned char>(encoded_char)))) {
        if (word.length() > 10000) {
          std::cerr << "Suspiciously long word — maybe no split occurred?" << std::endl;
          word.clear();
          word_as_list.clear();
        }
        if (!word.empty()) {
          cms.add(word);
          uint32_t count = cms.estimate(word);
          if (count >= 5) {
            add_word(word, word_as_list);

          }

          word.clear();
          word_as_list.clear();
        }
        last_char = 0; // Reset last_char lter punctuation
      } else {
        word += char_s;
        word_as_list.push_back(char_to_token[encoded_char]);
      }

      charsRead++;
      c_index = -1; // Reset after completing a character
    }
  };
  while (file.read(buffer, sizeof(buffer))) {
    chunkSize = file.gcount();
    for (size_t i = 0; i < chunkSize; i++) {
      processCharacter(buffer[i]);
    }
  }
  chunkSize = file.gcount();
  for (size_t i = 0; i < chunkSize; i++) {
    processCharacter(buffer[i]);
  }
  if (!word.empty()) {
    cms.add(word);
    uint32_t count = cms.estimate(word);
    if (count >= 5) {
      add_word(word, word_as_list);
    }
  }
  return charsRead;
}
void Trainer::initialize_queue_from_pairs() {
  std::vector<FrequencyPair> heap;
  for (const auto &[pair, freq] : pair_frequencies) {
    if (freq >= 2) {
      heap.emplace_back(pair, freq);
    }
  }
  std::make_heap(heap.begin(), heap.end(), PairFrequencyOrder());
  std::priority_queue<FrequencyPair, std::vector<FrequencyPair>,
                      PairFrequencyOrder>
      pq(PairFrequencyOrder(), std::move(heap));
  freq_pq = std::move(pq);
}
std::vector<uint32_t> Trainer::create_vocab_entry(BytePair bp) {
  std::vector<uint32_t> combined;
  std::vector<uint32_t> first = vocab[bp.first];
  std::vector<uint32_t> second = vocab[bp.second];
  combined.reserve(first.size() + second.size());
  combined = first;
  combined.insert(combined.end(), second.begin(), second.end());
  return combined;
}
size_t Trainer::train() {
  initialize_queue_from_pairs();
  vocab.resize(char_to_token.size());

  for (const auto &[character, idx] : char_to_token) {
    vocab[idx] = {character};
  }
  // Debug info before starting
  while ((vocab.size() < max_vocab_size) && !freq_pq.empty()) {
    auto [pair, stored_freq] = freq_pq.top();

    freq_pq.pop();
    uint32_t real_frequency = pair_frequencies[pair];

    if (real_frequency < 2) {
      continue;
    }
    if (real_frequency != stored_freq) {
      FrequencyPair new_frequency = std::make_pair(pair, real_frequency);
      freq_pq.push(new_frequency);
      continue;
    }

    uint32_t merged_id = vocab.size();
    vocab.push_back(create_vocab_entry(pair));

    std::set<BytePair> new_entries;

    for (const uint32_t word_id : pair_to_word[pair]) {
      uint32_t count = word_count[word_id];
      auto &word = words[word_id];
      std::vector<uint32_t> new_word;
      new_word.reserve(word.size());

      if(word.empty())
        continue;
      for (size_t i = 0; i < (word.size() - 1); i++) {
        uint32_t current_char = word[i];
        bool next_char_exists = i + 1 < word.size();
        bool next_next_char_exists = i + 2 < word.size();
        bool previous_char_eixsts = i >= 1;

        if (current_char == pair.first && next_char_exists &&
            word[i + 1] == pair.second) {
          // we found a pair
          if (previous_char_eixsts) {
            BytePair prev_left = std::pair(word[i - 1], word[i]);
            pair_frequencies[prev_left] -= count;
            BytePair left = std::pair(word[i - 1], merged_id);
            pair_frequencies[left] += count;
            new_entries.insert(left);
            pair_to_word[left].insert(word_id);
          }
          new_word.push_back(merged_id);
          if (next_next_char_exists) {
            BytePair prev_right = std::pair(word[i + 1], word[i + 2]);
            pair_frequencies[prev_right] -= count;
            BytePair right = std::pair(merged_id, word[i + 2]);
            pair_frequencies[right] += count;
            new_entries.insert(right);
            pair_to_word[right].insert(word_id);
          }
          // we merged two tokens, so we need to skip ahead
          i++;
        } else {
          new_word.push_back(word[i]);
        }
      }

      word = std::move(new_word);
    }
    // update queue
    for (auto new_pair : new_entries) {
      uint32_t frequency = pair_frequencies[new_pair];
      if (frequency < 2)
        continue;
      FrequencyPair pf = std::make_pair(new_pair, frequency);
      freq_pq.push(pf);
    }
  }
  return vocab.size();
}

void Trainer::save_vocab(const std::string &filename) {
  std::ofstream outfile(filename, std::ios::binary);
  if (!outfile) {
    std::cerr << "Error opening file for writing: " << filename << std::endl;
    return;
  }

  // Header
  uint32_t magic_number = 0xBEEFBABE;   // Unique identifier
  uint8_t version = 1;                  // Version number
  uint8_t data_type = sizeof(uint32_t); // Current size of codepoints
  uint32_t alphabet_size =
      std::count_if(vocab.begin(), vocab.end(),
                    [](const auto &toks) { return toks.size() == 1; });
  uint16_t reserved = 0; // Future-proof padding

  // Write header
  outfile.write(reinterpret_cast<const char *>(&magic_number),
                sizeof(magic_number));
  outfile.write(reinterpret_cast<const char *>(&version), sizeof(version));
  outfile.write(reinterpret_cast<const char *>(&data_type), sizeof(data_type));
  outfile.write(reinterpret_cast<const char *>(&alphabet_size),
                sizeof(alphabet_size));
  outfile.write(reinterpret_cast<const char *>(&reserved), sizeof(reserved));

  // Write the vocabulary size
  uint32_t vocab_size = vocab.size();
  outfile.write(reinterpret_cast<char *>(&vocab_size), sizeof(vocab_size));

  // Write vocab entries
  for (const auto &codepoints : vocab) {
    uint32_t length = codepoints.size();
    outfile.write(reinterpret_cast<char *>(&length), sizeof(length));
    outfile.write(reinterpret_cast<const char *>(codepoints.data()),
                  length * sizeof(uint32_t));
  }

  outfile.close();
  std::cout << "Vocabulary saved to: " << filename << std::endl;
}
} // namespace spellbit
