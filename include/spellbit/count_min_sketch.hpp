#ifndef COUNT_MIN_SKETCH_H
#define COUNT_MIN_SKETCH_H
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
namespace spellbit {
class CountMinSketch {
  std::vector<std::vector<uint32_t>> table;
  std::hash<std::string> hasher;
  size_t depth, width;

public:
  CountMinSketch(size_t d, size_t w)
      : depth(d), width(w), table(d, std::vector<uint32_t>(w, 0)) {}

  void add(const std::string &item);
  uint32_t estimate(const std::string &item);
};

} // namespace spellbit
#endif
