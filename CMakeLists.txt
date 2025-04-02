cmake_minimum_required(VERSION 3.12)
project(spellbit)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------
# Include External Libraries
# --------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/json
)

# --------------------------
# Core Library
# --------------------------
add_library(spellbit_trainer STATIC
    core/trainer.cpp
)
add_library(spellbit_tokenizer STATIC
    core/tokenizer.cpp
)


# --------------------------
# CLI Executable
# --------------------------
add_executable(spellbit
    cli/main.cpp
)

target_link_libraries(spellbit
    spellbit_trainer
    spellbit_tokenizer
)

