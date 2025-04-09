#ifndef SPELLBIT_HELPERS
#define SPELLBIT_HELPERS

#include <cstdint>
namespace spellbit {

uint32_t utf8_to_uint32(const char *bytes, int length);
int8_t get_utf_char_size(unsigned char byte);

} // namespace spellbit
#endif
