find_package(Doxygen
    REQUIRED dot)

if(DOXYGEN_FOUND)
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
    set(DOXYGEN_EXCLUDE_PATTERNS */third_party/* */build/* */test/*)

    doxygen_add_docs(
        doxygen
        ${PROJECT_SOURCE_DIR}
        COMMENT "Generate Doxygen"
    )

    add_custom_command(TARGET doxygen
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html
        ${CMAKE_CURRENT_SOURCE_DIR}/doxygen
        COMMENT "Copying Doxygen output to docs/doxygen")

else()
    message(WARNING "Doxygen not found, not generating docs")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Sphinx)
find_package(Inkscape)

file(GLOB_RECURSE SPHINX_SRC
    ${CMAKE_CURRENT_LIST_DIR}/user_guide/*.md
    ${CMAKE_CURRENT_LIST_DIR}/user_guide/*.png
    ${CMAKE_CURRENT_LIST_DIR}/user_guide/*.svg
)

if(Sphinx_FOUND AND Inkscape_FOUND)
    check_sphinx_extension("sphinx_rtd_theme")
    check_sphinx_extension("sphinxcontrib.inkscapeconverter")
    check_sphinx_extension("myst_parser")
    check_sphinx_extension("breathe")

    add_sphinx_document(
        user_guide
        SKIP_PDF
        CONF_FILE "${CMAKE_CURRENT_LIST_DIR}/user_guide/conf.py"
        ${SPHINX_SRC}
    )
else()
    message(WARNING "Sphinx not found, not generating docs")
endif()