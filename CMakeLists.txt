cmake_minimum_required(VERSION 3.12)
project(spellbit)
set(CMAKE_CXX_STANDARD 17)
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
    #${CMAKE_SOURCE_DIR}/external/json
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
add_library(spellbit_helpers STATIC
    core/helpers.cpp
)
add_library(spellbit_count_min_sketch STATIC
    core/count_min_sketch.cpp
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
    cli/main.cpp
)

target_link_libraries(spellbit
    spellbit_helpers
    spellbit_count_min_sketch
    spellbit_trainer
    spellbit_tokenizer
)

