cmake_minimum_required(VERSION 3.15)

find_package(Qt6 COMPONENTS Test Core Widgets Xml REQUIRED)

macro(create_qtest name)
    add_executable(${name} ${name}.cpp)
    add_test(${name} ${name})
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE Qt6::Core Qt6::Test)
endmacro()

# copy the data folder to the binary directory
file(COPY data/yosys/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/yosys/)
file(COPY data/routing/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/routing/)
file(COPY data/mainwindow/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/mainwindow/)

# include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/third_party)

# adding tests
create_qtest(tst_mainwindow)
target_link_libraries(tst_mainwindow PRIVATE diag Qt6::Widgets Qt6::Xml Qt6::Svg Qt6::SvgWidgets)

create_qtest(tst_yosys)
target_link_libraries(tst_yosys PRIVATE yosys Qt6::Svg Qt6::SvgWidgets)

create_qtest(tst_routing)
target_link_libraries(tst_routing PRIVATE symbol Qt6::Xml Qt6::Svg Qt6::SvgWidgets)