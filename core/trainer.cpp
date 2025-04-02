#include <cstddef>
#include <spellbit/trainer.hpp>
#include <iostream> 
#include <fstream> 

namespace spellbit {
int Trainer::addFile(std::string path) {
  std::ifstream file(path, std::ios::binary); 
  if (!file) {
    std::cerr << "Unable to open file: " << path << "\n";
    return -1;
  }

  char buffer[4096];
  int bytesRead = 0;
  int chunkSize;
  while(file.read(buffer, sizeof(buffer))) {
    chunkSize = file.gcount();
    bytesRead += chunkSize;
  }

  bytesRead += file.gcount();
  return bytesRead;
}
Trainer::~Trainer(){
}
Trainer::Trainer(){}
int Trainer::train() {
  return 0;
}
}


