cmake_minimum_required(VERSION 3.15)

# subdirectories
add_subdirectory(yosys)
add_subdirectory(routing)
add_subdirectory(symbol)
add_subdirectory(version)
add_subdirectory(third_party)

# set the project name
set(DIAG_LIB diag)

# add all the source files
set(DIAG_VIEW_SRC
    qnetlistgraphicsnode.cpp
    qnetlistgraphicspath.cpp
    qnetlistgraphicsellipse.cpp
    qnetlistgraphicstext.cpp
    qnetlistscidoublespin.cpp
    qnetlistscene.cpp
    qnetlistview.cpp
    qnetlisttabwidget.cpp
    netlisttab.cpp
    netlisttab.ui
    mainwindow.ui
    mainwindow.cpp
    dialogabout.cpp
    dialogabout.ui
    dialogsettings.cpp
    dialogsettings.ui
    dialogsearch.cpp
    dialogsearch.ui
    dialogproperties.cpp
    dialogproperties.ui
)

include_directories(${CMAKE_SOURCE_DIR}/src/third_party)

# Find required Qt packages
find_package(Qt6 COMPONENTS Core Widgets Xml SvgWidgets REQUIRED)

project(${DIAG_LIB}
    LANGUAGES CXX)

add_library(${DIAG_LIB} ${DIAG_VIEW_SRC})

target_link_libraries(${DIAG_LIB} PRIVATE Qt6::Core Qt6::Widgets Qt6::Xml Qt6::SvgWidgets)
target_link_libraries(${DIAG_LIB} PRIVATE yosys routing symbol version)
