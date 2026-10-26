cmake_minimum_required(VERSION 3.16)
project(mozzarella LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

option(MOZZARELLA_BUILD_TESTS "Build tests" OFF)
option(MOZZARELLA_BUILD_BENCHMARKS "Build benchmarks" OFF)

add_library(mozzarella_core
    src/evaluation.cpp
    src/log.cpp
    src/move_generation.cpp
    src/move.cpp
    src/perft.cpp
    src/position.cpp
    src/search.cpp
    src/timer.cpp
    src/tt.cpp
    src/utils.cpp
    src/zobrist.cpp
)

target_include_directories(mozzarella_core PUBLIC include)

add_executable(mozzarella src/uci.cpp)
target_link_libraries(mozzarella PRIVATE mozzarella_core)

if(MOZZARELLA_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(MOZZARELLA_BUILD_BENCHMARKS)
    add_subdirectory(bench)
endif()
