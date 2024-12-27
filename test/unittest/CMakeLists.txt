enable_testing()
find_package(GTest REQUIRED)

add_executable(unittest 
    unittest.cpp
    cases/vector.cpp
    cases/point.cpp
    cases/box.cpp
    cases/tree.cpp)
target_link_libraries(unittest 
                    PRIVATE orthtree
                    GTest::gtest GTest::gtest_main)
