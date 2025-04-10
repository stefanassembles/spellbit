#include <fstream>
#include <iostream>
#include <spellbit/tokenizer.hpp>

namespace spellbit {
void Trie::add_token(uint32_t encoded_char, uint32_t token_id) {
  char_to_token[encoded_char] = token_id;
}

void Trie::insert(const std::vector<uint32_t> &token, uint32_t id) {
  uint32_t current = 0;
  for (uint32_t codepoint : token) {
    uint32_t token_id = char_to_token[codepoint];
    if (token_id >= alphabet_size) {
      std::cerr << "Error: Codepoint " << token_id << " exceeds alphabet size "
                << alphabet_size << std::endl;
      return;
    }
    TrieNode &node = nodes[current];
    uint32_t &next = node.children[token_id];
    if (next == INVALID_INDEX) {
      next = nodes.size();
      nodes.emplace_back(alphabet_size); // Create a new node
    }
    current = next;
  }
  nodes[current].token_id = id;
}

std::optional<TokenMatch>
Trie::find_longest_match(const std::vector<uint32_t> &text, size_t start) {
  uint32_t current = 0;
  uint32_t longest_id = INVALID_TOKEN;
  size_t current_length = 0;
  size_t match_length = 0;

  for (size_t i = start; i < text.size(); ++i) {
    uint32_t encoded_char = text[i];
    uint32_t token = char_to_token[encoded_char];
    if (token >= alphabet_size ||
        nodes[current].children[token] == INVALID_INDEX)
      break;

    current = nodes[current].children[token];
    current_length++;

    if (nodes[current].token_id != INVALID_TOKEN) {
      longest_id = nodes[current].token_id;
      match_length = current_length;
    }
  }
  if (match_length > 0) {
    return TokenMatch{longest_id, match_length};
  }
  return std::nullopt;
}
bool Tokenizer::init(const std::string &vocab_filename) {
  std::ifstream infile(vocab_filename, std::ios::binary);
  if (!infile) {
    std::cerr << "Error: Unable to open file: " << vocab_filename << std::endl;
    return false;
  }

  uint32_t magic_number;
  uint8_t version;
  uint8_t data_type;
  uint32_t alphabet_size;
  uint16_t reserved;

  infile.read(reinterpret_cast<char *>(&magic_number), sizeof(magic_number));
  infile.read(reinterpret_cast<char *>(&version), sizeof(version));
  infile.read(reinterpret_cast<char *>(&data_type), sizeof(data_type));
  infile.read(reinterpret_cast<char *>(&alphabet_size), sizeof(alphabet_size));
  infile.read(reinterpret_cast<char *>(&reserved), sizeof(reserved));

  if (magic_number != 0xBEEFBABE) {
    std::cerr << "Error: Invalid magic number in file: " << vocab_filename
              << std::endl;
    return false;
  }
  if (version != 1) {
    std::cerr << "Error: Unsupported version: " << static_cast<int>(version)
              << std::endl;
    return false;
  }

  uint32_t vocab_size;
  infile.read(reinterpret_cast<char *>(&vocab_size), sizeof(vocab_size));

  trie = std::make_unique<Trie>(alphabet_size, vocab_size);

  uint32_t id = 0;
  for (uint32_t i = 0; i < vocab_size; ++i) {
    uint32_t length;
    infile.read(reinterpret_cast<char *>(&length), sizeof(length));
    std::vector<uint32_t> token(length);
    infile.read(reinterpret_cast<char *>(token.data()),
                length * sizeof(uint32_t));
    if (length == 1) {
      trie->add_token(token[0], id);
    }
    trie->insert(token, id);
    id++;
  }
  infile.close();
  return true;
}

std::vector<uint32_t> Tokenizer::tokenize(const std::string &filename) const {
  const size_t BUFFER_SIZE = 4096;
  char buffer[BUFFER_SIZE];
  char character[4];
  int8_t c_index = -1;
  int8_t max_size = 0;

  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Error: Unable to open file: " << filename << std::endl;
    return {};
  }

  std::vector<uint32_t> current;
  std::vector<uint32_t> tokens;

  while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
    size_t chunkSize = file.gcount();
    for (size_t i = 0; i < chunkSize; ++i) {
      char c = buffer[i];
      int8_t c_size = get_utf_char_size(c);

      if (c_index == -1) {
        if (c_size <= 0) {
          std::cerr << "Malformed or unexpected UTF-8 byte: "
                    << static_cast<int>(c) << "\n";
          c_index = -1;
          continue;
        }
        c_index = 0;
        max_size = c_size;
      }

      character[c_index++] = c;

      if (c_index == max_size) {
        uint32_t encoded_char = utf8_to_uint32(character, max_size);

        if ((encoded_char < 128) &&
            (std::isspace(static_cast<unsigned char>(encoded_char)) ||
             std::ispunct(static_cast<unsigned char>(encoded_char)))) {
          if (!current.empty()) {
            size_t pos = 0;
            while (pos < current.size()) {
              auto match = trie->find_longest_match(current, pos);
              if (match.has_value()) {
                tokens.push_back(match->token_id);
                pos += match->match_length;
              } else {
                tokens.push_back(current[pos]);
                pos++;
              }
            }
            current.clear();
          }
        } else {
          current.push_back(encoded_char);
        }
        c_index = -1;
      }
    }
  }

  if (!current.empty()) {
    size_t pos = 0;
    while (pos < current.size()) {
      auto match = trie->find_longest_match(current, pos);
      if (match.has_value()) {
        tokens.push_back(match->token_id);
        pos += match->match_length;
      } else {
        tokens.push_back(current[pos]);
        pos++;
      }
    }
  }

  return tokens;
}

} // namespace spellbit
