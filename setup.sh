#!/bin/bash

VERSION=0.9

BUILD_GENERATOR="Ninja"

TOOL="${0##*/}"
CMDLINE="${0##*/} $@"

# check if the QT_DIR environment variable is set
if [ -z "$QT_DIR" ]; then
    echo "Error: QT_DIR environment variable is not set"
    exit 1
fi

# check if the QT_DIR directory has a /lib/cmake/Qt6/qt.toolchain.cmake file
if [ ! -f "$QT_DIR/lib/cmake/Qt6/qt.toolchain.cmake" ]; then
    echo "Error: $QT_DIR/lib/cmake/Qt6/qt.toolchain.cmake file not found"
    echo "Please set the QT_DIR environment variable to a directory containing the Qt6 toolchain file"
    echo "For example: export QT_DIR=/path/to/Qt/6.2.0/gcc_64"
    exit 2
fi

# Arguments: <QT_DIR> <enable_doc> <enable_profiling> <enable_tests> <build_type>
setup_cmake()
{
    QT_DIR=$1
    ENABLE_DOC=$2
    ENABLE_PROFILING=$3
    ENABLE_TESTS=$4
    BUILD_TYPE=$5

    # change into the directory the script is located in
    cd "$(dirname "$0")"

    # create the build folder if it doesn't exist
    if [ ! -d "build" ]; then
        mkdir build
    fi

    cd build

    /usr/bin/cmake \
        -DCMAKE_BUILD_TYPE:STRING=$BUILD_TYPE \
        -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
        -DQT_DEBUG_FIND_PACKAGE=ON  \
        -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$QT_DIR/lib/cmake/Qt6/qt.toolchain.cmake \
        -DBUILD_WITH_DOCS:STRING=$([ $ENABLE_DOC -eq 1 ] && echo "ON" || echo "OFF") \
        -DBUILD_WITH_PROFILING:STRING=$([ $ENABLE_PROFILING -eq 1 ] && echo "ON" || echo "OFF") \
        -DBUILD_WITH_TESTS:STRING=$([ $ENABLE_TESTS -eq 1 ] && echo "ON" || echo "OFF") -G $BUILD_GENERATOR ..

}

usage(){
    echo "Usage: $TOOL [<options>] <Verilog files>"
    echo
    echo "Sets up the cmake build system for the OpenNetlistView project"
    echo
    echo "Options:"
    echo
    echo "  -d      enable documentation"
    echo
    echo "  -p      enable profiling"
    echo
    echo "  -t      enable tests"
    echo
    echo "  --btype <type>  set build type (Debug, Release, MinSizeRel, RelWithDebInfo)"
    echo "                  (default is Release)"
    echo
    echo "  -h      show this help"
    echo
}

error(){
    echo "Error: $1"
    echo
    usage
    exit 3
}

QT_DIR=$QT_DIR
ENABLE_DOC=0
ENABLE_PROFILING=0
ENABLE_TESTS=0
BUILD_TYPE="Release"
PARSE_OPTS="1"

while [[ $PARSE_OPTS == "1" ]]; do
    case "$1" in
        -d)
            ENABLE_DOC=1
            shift
            ;;
        -p)
            ENABLE_PROFILING=1
            shift
            ;;
        -t)
            ENABLE_TESTS=1
            shift
            ;;
        --btype)
            if [[ "$2" =~ ^(Debug|Release|MinSizeRel|RelWithDebInfo)$ ]]; then
                BUILD_TYPE="$2"
                shift 2
            else
                error "Invalid build type: $2"
            fi
            ;;
        -h)
            usage
            exit 0
            ;;
        -*)
            error "Unknown option: $1"
            ;;
        *)
            PARSE_OPTS="0"
            ;;
    esac
done

setup_cmake $QT_DIR $ENABLE_DOC $ENABLE_PROFILING $ENABLE_TESTS $BUILD_TYPE