enable_testing()

find_package(GTest REQUIRED)

# Add test executable
add_executable(unittests
    unittests.cpp
    cases/vector.cpp
    cases/point.cpp
    cases/box.cpp
    cases/tree.cpp
)

target_link_libraries(unittests
    PRIVATE 
        orthtree::orthtree
        GTest::gtest
        GTest::gtest_main
)

# Add test discovery
include(GoogleTest)
gtest_discover_tests(unittests
    EXTRA_ARGS "--gtest_color=yes"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
    