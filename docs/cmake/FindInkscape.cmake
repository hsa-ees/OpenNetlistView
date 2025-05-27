include(FindPackageHandleStandardArgs)

find_program(
    INKESCAPE_EXECUTABLE
    NAMES inkscape inkscape.exe)
mark_as_advanced(INKESCAPE_EXECUTABLE)

find_package_handle_standard_args(Inkscape DEFAULT_MSG INKESCAPE_EXECUTABLE)