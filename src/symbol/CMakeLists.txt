cmake_minimum_required(VERSION 3.15)

# set the project name
set(SYMBOL_LIB symbol)

# add all the source files
set(SYMBOL_SRC
    port.cpp
    symbol.cpp
    symbol_parser.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/third_party)

# Find required Qt packages
find_package(Qt6 COMPONENTS Core Xml Svg REQUIRED)

project(${SYMBOL_LIB}
    LANGUAGES CXX)

add_library(${SYMBOL_LIB} ${SYMBOL_SRC})

target_link_libraries(${SYMBOL_LIB} PRIVATE Qt6::Core Qt6::Xml Qt6::Svg)
target_link_libraries(${SYMBOL_LIB} PRIVATE cola)
