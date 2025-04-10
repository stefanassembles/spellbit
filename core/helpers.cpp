#include <spellbit/helpers.hpp>

namespace spellbit {
uint32_t utf8_to_uint32(const char *bytes, int length) {
  uint32_t codepoint = 0;
  if (length == 1) {
    codepoint = bytes[0];
  } else if (length == 2) {
    codepoint = ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
  } else if (length == 3) {
    codepoint = ((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) |
                (bytes[2] & 0x3F);
  } else if (length == 4) {
    codepoint = ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) |
                ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
  }
  return codepoint;
}

int8_t get_utf_char_size(unsigned char byte) {
  if ((byte & 0X80) == 0) {
    return 1;
  } else if ((byte & 0xe0) == 0xc0) {
    return 2;
  } else if ((byte & 0xf0) == 0xe0) {
    return 3;
  } else if ((byte & 0xf8) == 0xf0) {
    return 4;
  } else if ((byte & 0xc0) == 0x80) {
    return 0;
  } else {
    return -1;
  }
}

} 
