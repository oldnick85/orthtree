add_library(orthtree INTERFACE)
add_library(orthtree::orthtree ALIAS orthtree)
    
# Include directories for interface library
target_include_directories(orthtree INTERFACE .)
