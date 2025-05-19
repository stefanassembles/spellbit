cmake_minimum_required(VERSION 3.12)
project(spellbit)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(FetchContent)

# Include External Libraries
# --------------------------
include_directories(
    include/
    external/
)

# --------------------------
# Core Library
# --------------------------
add_library(spellbit_trainer STATIC
    src/trainer.cpp
)
add_library(spellbit_tokenizer STATIC
    src/tokenizer.cpp
)
add_library(spellbit_helpers STATIC
    src/helpers.cpp
)
add_library(spellbit_count_min_sketch STATIC
    src/count_min_sketch.cpp
)


target_link_libraries(spellbit_trainer
    spellbit_helpers
    spellbit_count_min_sketch
)
target_link_libraries(spellbit_tokenizer
    spellbit_helpers
)


# --------------------------
# CLI Executable
# --------------------------
add_executable(spellbit
    src/main.cpp
)
target_link_libraries(spellbit
    spellbit_helpers
    spellbit_count_min_sketch
    spellbit_trainer
    spellbit_tokenizer
)


add_executable(test_runner
	external/catch2/catch_amalgamated.cpp
	tests/test_runner.cpp
	tests/test_helpers.cpp

	)
target_include_directories(test_runner PRIVATE external/catch2)

