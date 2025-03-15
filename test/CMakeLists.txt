
add_executable(my_tests test_untyped.cpp)
target_include_directories(my_tests PRIVATE lib)
target_include_directories(my_tests PRIVATE ../src)

# CTest
enable_testing()
add_test(NAME doctest_tests COMMAND tests)
