cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(OpenNetlistView
    LANGUAGES CXX
    VERSION 1.0.0
    DESCRIPTION "An interactive schemetic viewer for Yosys hardware synthesis tool")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(QT_VERSION_MAJOR 6)

# Set _DEBUG flag for debug builds
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(_DEBUG)
endif()

# Find required Qt packages
find_package(Qt6 COMPONENTS Core Widgets Xml Svg SvgWidgets REQUIRED)

# add the source files in the src directory
add_subdirectory(src)

# add top level file to the project
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OpenNetlistView
        MANUAL_FINALIZATION
        main.cpp
        ${CMAKE_SOURCE_DIR}/resources/symbols/symbols.qrc
        ${CMAKE_SOURCE_DIR}/resources/icons/icons.qrc
        ${CMAKE_SOURCE_DIR}/resources/examples/examples.qrc
    )

# Define target properties for Android with Qt 6 as:
# set_property(TARGET OpenNetlistView APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(OpenNetlistView SHARED
            ${PROJECT_SOURCES}
        )

    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(OpenNetlistView
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(OpenNetlistView PRIVATE Qt6::Widgets Qt6::Core Qt6::Xml Qt6::Svg Qt6::SvgWidgets)
target_link_libraries(OpenNetlistView PRIVATE diag)
target_include_directories(OpenNetlistView PRIVATE ${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/third_party)

# link agains jemalloc in non-emscripten builds
# this prevents lockups when opening submodules
# on native linux systems
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
else()
    find_library(JEMALLOC_LIB jemalloc REQUIRED)

    message(STATUS ${JEMALLOC_LIB})

    if(JEMALLOC_LIB)
        target_link_libraries(OpenNetlistView PRIVATE ${JEMALLOC_LIB})
        message(STATUS "Using Jemalloc: " ${JEMALLOC_LIB})
    else()
        message(FATAL_ERROR "Jemalloc not found")
    endif()
endif()

# add tests
option(BUILD_WITH_TESTS "Build OpenNetlistView" OFF)

if(BUILD_WITH_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Enable profiling if the ENABLE_PROFILING option is set to ON
option(BUILD_WITH_PROFILING "This option will enable GNU profiling" OFF)

if(BUILD_WITH_PROFILING)
    # Add -pg flag to compiler and linker options
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
endif()

# Generate Docs
OPTION(BUILD_WITH_DOCS "Generate Docs" OFF)

IF(BUILD_WITH_DOCS)
    ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/docs)
ENDIF()

# MacOS specific settings
set_target_properties(OpenNetlistView PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.lukbau.OpenNetlistView

    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Linking required libraries
install(TARGETS OpenNetlistView
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(OpenNetlistView)
endif()
