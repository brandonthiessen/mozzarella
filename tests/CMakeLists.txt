include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.18.0
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test_perft test_perft.cpp)
target_link_libraries(test_perft
    PRIVATE
        mozzarella_core
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(test_perft)
