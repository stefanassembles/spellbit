cmake_minimum_required(VERSION 3.12)
project(spellbit)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
# Define a custom ultra-optimized build type
set(CMAKE_CXX_FLAGS_PRODUCTION "-Ofast -DNDEBUG -march=native -flto -fomit-frame-pointer -ffunction-sections -fdata-sections" CACHE STRING "Ultra-optimized flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_PRODUCTION "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections" CACHE STRING "Production linker flags" FORCE)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
# --------------------------
# Include External Libraries
# --------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/include
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

