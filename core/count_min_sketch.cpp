#include<spellbit/count_min_sketch.hpp>

namespace spellbit {
    void CountMinSketch::add(const std::string &item) {
        for (size_t i = 0; i < depth; ++i) {
            size_t hash = (std::hash<std::string>{}(item) + i * 0x9e3779b9) % width;
            table[i][hash]++;
        }
    }

   uint32_t CountMinSketch::estimate(const std::string &item) {
        uint32_t min = UINT32_MAX;
        for (size_t i = 0; i < depth; ++i) {
            size_t hash = (std::hash<std::string>{}(item) + i * 0x9e3779b9) % width;
            min = std::min(min, table[i][hash]);
        }
        return min;
    }

}

