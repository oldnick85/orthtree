#!/bin/bash
set -e

# Parse command line arguments
ACTION=${1:-"build"}
BUILD_TYPE=${2:-"Release"}
BUILD_DIR=${3:-"build-linux"}

echo "Action: $ACTION, Build type: $BUILD_TYPE, Build directory: $BUILD_DIR"

case $ACTION in
    "build")
        echo "Building project for Linux (Ubuntu 25.10)..."

        cd /workspace

        # Create build directory and navigate to it
        mkdir -p $BUILD_DIR
        
        cd $BUILD_DIR
        
        # Configure CMake project
        cmake .. -DBUILD_ALL=ON -DENABLE_CLANG_TIDY=ON -DCMAKE_BUILD_TYPE=$BUILD_TYPE
        
        # Build the project
        cmake --build . --verbose
        
        ;;
    
    "test")
        echo "Running unit tests for Linux..."

        cd /workspace
        cd $BUILD_DIR/tests/unittests
        
        # Run tests with CTest
        ctest --output-on-failure -C $BUILD_TYPE -V
        ;;
    
    "format")
        echo "Checking code formatting with clang-format..."
        
        # Check formatting without making changes
        find src test -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | \
            xargs clang-format-20 --dry-run --Werror --style=file
        
        echo "✅ All files are properly formatted!"
        ;;
    
    "tidy")
        echo "Running static analysis with clang-tidy..."
        
        # Generate compile_commands.json
        cd /workspace

        cmake -B $BUILD_DIR -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_CLANG_TIDY="clang-tidy;--header-filter=.*"

        find examples -name "*.cpp" | xargs clang-tidy -p $BUILD_DIR --config-file=.clang-tidy
        find src -name "*.h" | xargs clang-tidy -p $BUILD_DIR --config-file=.clang-tidy
        find tests -name "*.cpp" | xargs clang-tidy -p $BUILD_DIR --config-file=.clang-tidy

        echo "✅ Static analysis completed successfully!"
        ;;
    
    "all")
        echo "Running full CI pipeline..."
        /usr/local/bin/entrypoint.sh format
        /usr/local/bin/entrypoint.sh build $BUILD_TYPE $BUILD_DIR
        /usr/local/bin/entrypoint.sh tidy
        /usr/local/bin/entrypoint.sh test $BUILD_TYPE $BUILD_DIR
        ;;
    
    *)
        echo "Available actions: build, test, format, tidy, all"
        echo "Usage: entrypoint.sh [action] [build_type] [build_dir]"
        exit 1
        ;;
esac