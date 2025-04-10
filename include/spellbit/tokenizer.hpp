#ifndef SPELLBIT_TOKENIZER_HPP
#define SPELLBIT_TOKENIZER_HPP
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <spellbit/helpers.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace spellbit {

struct TokenMatch {
  uint32_t token_id;
  size_t match_length;
};

class Trie {
private:
  std::unordered_map<uint32_t, uint32_t> char_to_token;
  static constexpr uint32_t INVALID_INDEX =
      std::numeric_limits<uint32_t>::max();
  static constexpr uint32_t INVALID_TOKEN =
      std::numeric_limits<uint32_t>::max() - 1;
  struct TrieNode {
    uint32_t token_id = INVALID_TOKEN;
    std::vector<uint32_t> children;

    explicit TrieNode(size_t alphabet_size)
        : children(alphabet_size, INVALID_INDEX), token_id(INVALID_TOKEN) {}
  };

public:
  size_t alphabet_size;
  std::vector<TrieNode> nodes;

  Trie(size_t alphabet_size, size_t vocab_size) : alphabet_size(alphabet_size) {
    nodes.reserve(vocab_size + 1);
    nodes.emplace_back(alphabet_size); // Root node
  }

  void add_token(uint32_t encoded_char, uint32_t token_id);
  void insert(const std::vector<uint32_t> &token, uint32_t id);
  std::optional<TokenMatch>
  find_longest_match(const std::vector<uint32_t> &text, size_t start);
};

class Tokenizer {
private:
  std::unique_ptr<Trie> trie;
  bool is_initialized = false;

public:
  bool init(const std::string &vocab_filename);

  std::vector<uint32_t> tokenize(const std::string &filename) const;
};

} // namespace spellbit
#endif
