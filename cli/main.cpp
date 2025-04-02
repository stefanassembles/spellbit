#include <iostream>
#include<string>
#include <spellbit/trainer.hpp>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "USAGE: spellbit [FILENAME]" << std::endl;
    return 1; 
  }

  

  std::string file_name(argv[1]);

  std::cout << "File_name: " << file_name << std::endl;
  spellbit::Trainer trainer;
  int bytes = trainer.addFile(file_name);
  std::cout << "Read: " << bytes << "bytes \n";




  return 0;
}
