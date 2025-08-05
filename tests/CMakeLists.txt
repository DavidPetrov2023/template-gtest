# Enable CTest
enable_testing()

# Define test executable
add_executable(tests test_counter.cpp)

# Include path to main library headers
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Link test binary with GoogleTest and your library
target_link_libraries(tests gtest_main counter)

# Automatically discover and register tests with CTest
include(GoogleTest)
gtest_discover_tests(tests)
