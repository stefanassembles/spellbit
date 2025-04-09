#include <iostream>
#include<string>
#include <spellbit/trainer.hpp>
#include <spellbit/tokenizer.hpp>
#include <chrono>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "USAGE: spellbit [FILENAME]" << std::endl;
    //return 1; 
  }
  std::string file_name(argv[1]);
  size_t max_vocab = 56000;
  spellbit::Trainer trainer(max_vocab);
  std::cout << "Reading file " << file_name << " ... ";
  auto first_time = std::chrono::high_resolution_clock::now();

  auto last_time = std::chrono::high_resolution_clock::now();
  int bytes = trainer.add_file(file_name);
  auto current_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time).count();

  std::cout << "[OK]" << std::endl;
  float mb_per_sec = ((float) bytes / (1024.0*1024.0)) / ((float) duration / 1000.0);

  float days_per_terrabyte = (1024.0*1024.0) / mb_per_sec / 60.0 / 60.0 / 24.0;
  std::cout << "Total: " << duration << " ms , MB/sec : " << mb_per_sec << ", days per Terrabyte: " << days_per_terrabyte << std::endl;
  std::cout << "Merging ... ";
  last_time = std::chrono::high_resolution_clock::now();
  auto tokens = trainer.train();
  current_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time).count();

  std::cout << "[OK]" << std::endl;
  float time_per_1000_tokens = float(duration) / float(max_vocab) * 1000; 
  std::cout << "Total: " << duration << " ms, ms / 1000 tokens : " << time_per_1000_tokens << std::endl;
  std::cout << "Final tokens: " << tokens << std::endl;

  std::string file("train.bpe");
  trainer.save_vocab(file);
  last_time = std::chrono::high_resolution_clock::now();
  std::cout << "Tokenizing ... ";
  spellbit::Tokenizer tokenizer = spellbit::Tokenizer();
  tokenizer.init(file);
  auto throw_away = tokenizer.tokenize(file_name);
  current_time = std::chrono::high_resolution_clock::now();
  std::cout << "[OK]" << std::endl;
  time_per_1000_tokens = float(duration) / float(max_vocab) * 1000;
  std::cout << "Total: " << duration << " ms, ms / 1000 tokens : " << time_per_1000_tokens << std::endl;
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - first_time).count();
  auto seconds = (double) duration / 1000.0;
  auto minutes = seconds / 60.0;
  std::cout << "Overall it took: " << seconds << " secs, or " << minutes << " minutes" << std::endl;
}
