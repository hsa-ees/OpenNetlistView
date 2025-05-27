set(ENV{GIT_DIR} ${DIAGVIEWER_SRC_DIR}/.git)
execute_process(COMMAND git describe --tags --exclude "continuous"
                OUTPUT_VARIABLE GIT_REV
                ERROR_QUIET)

# Check whether we got any revision (which isn't
# always the case, e.g. when someone downloaded a zip
# file from Github instead of a checkout)
if ("${GIT_REV}" STREQUAL "")
    set(DIAGVIEWER_GIT_VERSION "N/A")
else()
    string(STRIP "${GIT_REV}" DIAGVIEWER_GIT_VERSION)
endif()

set(VERSION_FILE ${DIAGVIEWER_SRC_DIR}/src/version/gen_versionnumber.h)

# New version file content
set(VERSION "#define DIAGVIEWER_GIT_VERSION \"${DIAGVIEWER_GIT_VERSION}\"")

# Only rewrite the version file if a change is seen
if(EXISTS ${VERSION_FILE})
    file(READ ${VERSION_FILE} OLD_VERSION)
else()
    set(OLD_VERSION "")
endif()

if (NOT "${VERSION}" STREQUAL "${OLD_VERSION}")
    file(WRITE ${VERSION_FILE} "${VERSION}")
endif()