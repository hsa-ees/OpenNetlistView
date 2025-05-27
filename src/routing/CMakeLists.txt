cmake_minimum_required(VERSION 3.15)

# set the project name
set(ROUTING_LIB routing)

# add all the source files
set(ROUTING_SRC
    router.cpp
    cola_router.cpp
    avoid_router.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/third_party)

# Find required Qt packages
find_package(Qt6 COMPONENTS Core Xml Svg SvgWidgets REQUIRED)

project(${ROUTING_LIB}
    LANGUAGES CXX)

add_library(${ROUTING_LIB} ${ROUTING_SRC})

target_link_libraries(${ROUTING_LIB} PRIVATE Qt6::Core Qt6::Xml Qt6::Svg Qt6::SvgWidgets)
target_link_libraries(${ROUTING_LIB} PRIVATE avoid cola yosys symbol topology)
