#ifndef SPELLBIT_TRAINER_HPP
#define SPELLBIT_TRAINER_HPP

#include <cstddef>
#include <string>

namespace spellbit {
class Trainer {
private:
  size_t n = 0;
  int8_t get_utf_char_size(unsigned char byte);

public:
  Trainer();
  ~Trainer();

  int addFile(std::string path);
  int train(); 
};

}
#endif

