# 🪄 spellbit

> A tiny spell for turning words into meaning.

**spellbit** is a fast, lightweight Byte Pair Encoding (BPE) tokenizer written in modern C++.  
It provides both a command-line interface (CLI) and Python bindings via pybind11 for easy integration into machine learning workflows.

---

## 🚀 Build & Install

### 🧩 Build CLI (CMake)

```bash
mkdir build && cd build
cmake ..
make
./spellbit-cli --help
```

---

## 🧱 Project Structure

- `src/` – Core C++ BPE logic
- `include/` – Public headers
- `cli/` – Command-line interface built on top of the core
- `py/` – Python bindings and packaging setup
- `tests/` – C++ and Python test suites



