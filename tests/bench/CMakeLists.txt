enable_testing()

add_executable(bench 
    bench.cpp
    cases/tree.cpp)
target_link_libraries(bench 
                    PRIVATE orthtree)
