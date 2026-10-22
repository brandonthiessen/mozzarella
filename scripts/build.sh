#!/usr/bin/env bash

set -e

CONFIG="release"
BUILD_TESTS=OFF
BUILD_BENCHMARKS=OFF

for ARG in "$@"; do
    case "$ARG" in
        debug)
            CONFIG="debug"
            ;;
        release)
            CONFIG="release"
            ;;
        tests)
            BUILD_TESTS=ON
            ;;
        bench|benchmarks)
            BUILD_BENCHMARKS=ON
            ;;
        all)
            BUILD_TESTS=ON
            BUILD_BENCHMARKS=ON
            ;;
        *)
            echo "Unknown argument: $ARG"
            echo "Usage: $0 [debug|release] [tests] [bench|benchmarks] [all]"
            exit 1
            ;;
    esac
done

BUILD_DIR="build/$CONFIG"

case "$CONFIG" in
    debug)   CMAKE_CONFIG="Debug" ;;
    release) CMAKE_CONFIG="Release" ;;
esac

cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$CMAKE_CONFIG" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DMOZZARELLA_BUILD_TESTS="$BUILD_TESTS" \
    -DMOZZARELLA_BUILD_BENCHMARKS="$BUILD_BENCHMARKS"

ln -sf "$BUILD_DIR/compile_commands.json" compile_commands.json

cmake --build "$BUILD_DIR" --parallel

ln -sf "$BUILD_DIR/mozzarella" mozzarella
