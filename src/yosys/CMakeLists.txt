cmake_minimum_required(VERSION 3.15)

# set the project name
set(YOSYS_LIB yosys)

# add all the source files
set(YOSYS_SRC
    parser.cpp
    diagram.cpp
    component.cpp
    "path.cpp"
    node.cpp
    port.cpp
    module.cpp
    netname.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/third_party)

# Find required Qt packages
find_package(Qt6 COMPONENTS Core Svg SvgWidgets REQUIRED)

project(${YOSYS_LIB}
    LANGUAGES CXX)

add_library(${YOSYS_LIB} ${YOSYS_SRC})

target_link_libraries(${YOSYS_LIB} PRIVATE Qt6::Core Qt6::Svg Qt6::SvgWidgets)
target_link_libraries(${YOSYS_LIB} PRIVATE symbol avoid diag)
