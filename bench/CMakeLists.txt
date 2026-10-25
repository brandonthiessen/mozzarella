
include(FetchContent)

FetchContent_Declare(
    google_benchmark
    GIT_REPOSITORY https://github.com/google/benchmark
    GIT_TAG v1.9.1
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(google_benchmark)

add_executable(bench_move bench_move.cpp)
target_link_libraries(
    bench_move
    PRIVATE mozzarella_core benchmark::benchmark_main
)

add_executable(bench_movegen bench_movegen.cpp)
target_link_libraries(
    bench_movegen
    PRIVATE mozzarella_core benchmark::benchmark_main
)

add_executable(bench_perft bench_perft.cpp)
target_link_libraries(
    bench_perft
    PRIVATE mozzarella_core benchmark::benchmark_main
)

add_executable(bench_search bench_search.cpp)
target_link_libraries(
    bench_search
    PRIVATE mozzarella_core benchmark::benchmark_main
)
