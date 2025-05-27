set(_SPHINX_SCRIPT_DIR ${CMAKE_CURRENT_LIST_DIR})

include(FindPackageHandleStandardArgs)

# We are likely to find Sphinx near the Python interpreter
find_package(PythonInterp)

if(PYTHONINTERP_FOUND)
    get_filename_component(_PYTHON_DIR "${PYTHON_EXECUTABLE}" DIRECTORY)
    set(
        _PYTHON_PATHS
        "${_PYTHON_DIR}"
        "${_PYTHON_DIR}/bin"
        "${_PYTHON_DIR}/Scripts")
endif()

find_program(
    SPHINX_EXECUTABLE
    NAMES sphinx-build sphinx-build.exe
    HINTS ${_PYTHON_PATHS})
mark_as_advanced(SPHINX_EXECUTABLE)

find_package_handle_standard_args(Sphinx DEFAULT_MSG SPHINX_EXECUTABLE)

# If finding Sphinx fails, there is no use in defining
# add_sphinx_document, so return early
if(NOT Sphinx_FOUND)
    return()
endif()

# Function to check if a Sphinx extension is installed
function(check_sphinx_extension EXTENSION_NAME)
    execute_process(
        COMMAND "${PYTHON_EXECUTABLE}" -c "import ${EXTENSION_NAME}"
        RESULT_VARIABLE _result
        OUTPUT_QUIET
        ERROR_QUIET)

    if(NOT _result EQUAL 0)
        message(FATAL_ERROR "Required Sphinx extension '${EXTENSION_NAME}' is not installed.")
    endif()
endfunction()

# add_sphinx_document(
# <name>
# CONF_FILE <conf-py-filename>
# [SKIP_HTML] [SKIP_PDF]
# <src-file>...)
#
# Function for creating Sphinx documentation targets.
function(add_sphinx_document TARGET_NAME)
    # parsing cmd arguments
    cmake_parse_arguments(
        ${TARGET_NAME}
        "SKIP_HTML;SKIP_PDF"
        "CONF_FILE"
        ""
        ${ARGN})

    get_filename_component(SRCDIR "${${TARGET_NAME}_CONF_FILE}" DIRECTORY)
    set(INTDIR "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}/source")
    set(OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}/build")

    string(TIMESTAMP SPHINX_TARGET_YEAR "%Y" UTC)

    add_custom_command(
        OUTPUT "${INTDIR}/conf.py"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${INTDIR}"
        COMMAND
        "${CMAKE_COMMAND}"
        "-DCONFIGURE_FILE_IN=${${TARGET_NAME}_CONF_FILE}"
        "-DCONFIGURE_FILE_OUT=${INTDIR}/conf.py"
        "-DSPHINX_TARGET_NAME=${TARGET_NAME}"
        "-DSPHINX_TARGET_YEAR=${SPHINX_TARGET_YEAR}"
        -P "${_SPHINX_SCRIPT_DIR}/BuildTimeConfigureFile.cmake"
        DEPENDS "${${TARGET_NAME}_CONF_FILE}")

    set(SPHINX_DEPENDS "${INTDIR}/conf.py")

    foreach(DOCFILE ${${TARGET_NAME}_UNPARSED_ARGUMENTS})
        get_filename_component(DOCFILE_INTDIR "${DOCFILE}" DIRECTORY)
        string(
            REPLACE
            "${SRCDIR}" "${INTDIR}"
            DOCFILE_INTDIR "${DOCFILE_INTDIR}")

        get_filename_component(DOCFILE_DEST "${DOCFILE}" NAME)
        set(DOCFILE_DEST "${DOCFILE_INTDIR}/${DOCFILE_DEST}")

        add_custom_command(
            OUTPUT "${DOCFILE_DEST}"
            COMMAND
            "${CMAKE_COMMAND}" -E make_directory "${DOCFILE_INTDIR}"
            COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different
            "${DOCFILE}" "${DOCFILE_DEST}"
            DEPENDS "${DOCFILE}")

        list(APPEND SPHINX_DEPENDS "${DOCFILE_DEST}")
    endforeach()

    set(TARGET_DEPENDS)

    if(NOT ${TARGET_NAME}_SKIP_HTML)
        add_custom_command(
            OUTPUT "${OUTDIR}/html.stamp"

            # Create the _static directory required by Sphinx in case it
            # wasn't added as one of the source files
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${INTDIR}/_static"
            COMMAND "${SPHINX_EXECUTABLE}" -M html "${INTDIR}" "${OUTDIR}"
            COMMAND "${CMAKE_COMMAND}" -E touch "${OUTDIR}/html.stamp"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "${OUTDIR}/html" "${CMAKE_CURRENT_SOURCE_DIR}/user_guide/build/html"
            DEPENDS ${SPHINX_DEPENDS})

        list(APPEND TARGET_DEPENDS "${OUTDIR}/html.stamp")
    endif()

    if(NOT ${TARGET_NAME}_SKIP_PDF)
        find_program(LATEXMK_EXECUTABLE NAMES latexmk)
        find_package(LATEX COMPONENTS PDFLATEX)

        if(LATEXMK_EXECUTABLE)
            add_custom_command(
                OUTPUT "${OUTDIR}/latex/${TARGET_NAME}.pdf"
                COMMAND "${SPHINX_EXECUTABLE}" -M latexpdf "${INTDIR}" "${OUTDIR}"
                COMMAND "${CMAKE_COMMAND}" -E copy "${OUTDIR}/latex/${TARGET_NAME}.pdf" "${CMAKE_CURRENT_SOURCE_DIR}/user_guide/build"
                DEPENDS ${SPHINX_DEPENDS})

            list(APPEND TARGET_DEPENDS "${OUTDIR}/latex/${TARGET_NAME}.pdf")
        else()
            message(WARNING "latexmk not found. Trying to use pdflatex directly.")
        endif()

        if(LATEX_FOUND AND NOT LATEXMK_EXECUTABLE)
            add_custom_command(
                OUTPUT "${OUTDIR}/latex/${TARGET_NAME}.tex"
                COMMAND "${SPHINX_EXECUTABLE}" -M latex "${INTDIR}" "${OUTDIR}"
                DEPENDS ${SPHINX_DEPENDS})

            add_custom_command(
                OUTPUT "${OUTDIR}/latex/${TARGET_NAME}.pdf"

                # Three times' the charm for PdfLaTeX to get all xrefs right
                COMMAND "${PDFLATEX_COMPILER}" "${TARGET_NAME}.tex"
                COMMAND "${PDFLATEX_COMPILER}" "${TARGET_NAME}.tex"
                COMMAND "${PDFLATEX_COMPILER}" "${TARGET_NAME}.tex"
                COMMAND "${CMAKE_COMMAND}" -E copy "${OUTDIR}/latex/${TARGET_NAME}.pdf" "${CMAKE_CURRENT_SOURCE_DIR}/user_guide/build"
                WORKING_DIRECTORY "${OUTDIR}/latex"
                DEPENDS "${OUTDIR}/latex/${TARGET_NAME}.tex")

            list(APPEND TARGET_DEPENDS "${OUTDIR}/latex/${TARGET_NAME}.pdf")
        elseif(NOT LATEX_FOUND AND NOT LATEXMK_EXECUTABLE)
            message(WARNING "pdflatex not found. Not building PDF documentation.")
        endif()
    endif()

    add_custom_target(
        ${TARGET_NAME}
        DEPENDS ${TARGET_DEPENDS})

    if(NOT TARGET doc)
        add_custom_target(doc)
    endif()

    add_dependencies(doc ${TARGET_NAME})
endfunction()