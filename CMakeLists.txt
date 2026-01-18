cmake_minimum_required(VERSION 3.22)

project(orthtree
    VERSION 1.0.0
    DESCRIPTION "Tree bisection of n-dimensional area"
    LANGUAGES CXX
    HOMEPAGE_URL "https://github.com/oldnick85/orthtree")

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS 
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Modern way to set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Common warning flags for GCC and Clang
add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-system-headers)
    
# Additional warnings
add_compile_options(-Wshadow -Wnon-virtual-dtor -Wold-style-cast)
add_compile_options(-Wcast-align -Wunused -Woverloaded-virtual)
add_compile_options(-Wconversion -Wsign-conversion)
    
# Build type specific flags using generator expressions
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

option(BUILD_ALL "Build all targets (tests, examples, docs)" OFF)

option(BUILD_TESTING "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_DOCS "Build documentation" OFF)

if(BUILD_ALL)
    set(BUILD_TESTING ON CACHE BOOL "Build tests" FORCE)
    set(BUILD_EXAMPLES ON CACHE BOOL "Build examples" FORCE)
    set(BUILD_DOCS ON CACHE BOOL "Build documentation" FORCE)
    
    message(STATUS "BUILD_ALL enabled: All targets will be built")
endif()
    
# Sanitizers (optional, can be enabled via option)
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fno-omit-frame-pointer -fsanitize=address,undefined)
    add_link_options(-fno-omit-frame-pointer -fsanitize=address,undefined)
endif()

# Optional clang-tidy integration
option(ENABLE_CLANG_TIDY "Enable clang-tidy analysis" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY 
            ${CLANG_TIDY_EXE}
            --config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy
        )
        message(STATUS "clang-tidy enabled: ${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy requested but not found")
    endif()
endif()

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add subdirectories
add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(BUILD_DOCS)
    add_subdirectory(doc)
endif()
