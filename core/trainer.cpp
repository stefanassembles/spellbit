#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <spellbit/trainer.hpp>

namespace spellbit {
int8_t Trainer::get_utf_char_size(unsigned char byte) {
  if ((byte & 0X80) == 0) {
    // 0x0xxxxx
    return 1;
  } else if ((byte & 0xe0) == 0xc0) {
    // 0x110xxxx
    return 2;
  } else if ((byte & 0xf0) == 0xe0) {
    // 0x1110
    return 3;
  } else if ((byte & 0xf8) == 0xf0) {
    // 0x11110
    return 4;
  } else if ((byte & 0xc0) == 0x80) {
    // 0x10 - continuation byte
    return 0;
  } else {
    // error
    return -1;
  }
}
int Trainer::addFile(std::string path) {
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
  auto processCharacter = [&](char c) {
    int8_t c_size = get_utf_char_size(c);
    if (c_index == -1) {
      c_index = 0;
      max_size = c_size;
    }
    character[c_index++] = c;
    if (c_index == max_size) {
      std::string char_s(character, max_size);
      std::cout << char_s << "\n";
      charsRead++;
      c_index = -1;
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
  return charsRead;
}
Trainer::~Trainer() {}
Trainer::Trainer() {}
int Trainer::train() { return 0; }
} // namespace spellbit
