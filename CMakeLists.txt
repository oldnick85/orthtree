cmake_minimum_required(VERSION 3.22)

project(orthtree
    VERSION 1.0.0
    DESCRIPTION "Tree bisection of n-dimensional area"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message(${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2") # -fno-rtti

#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

message(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_CLANG_TIDY "clang-tidy;--config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy")

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(examples)
add_subdirectory(doc)