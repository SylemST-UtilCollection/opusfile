
#       OPUS_INCLUDE_DIRS      Header file directories
#       OPUS_LIBRARIES         Archive/shared objects
#       OPUS_FOUND         Archive/shared objects

find_path(OPUS_INCLUDE_DIR
    NAMES
        "opus/opus.h"
    PATHS
        "/usr"
        "/opt"
        "/"
    PATH_SUFFIXES
        "dynamic/include/opus"
        "local"
        "include"
        "external"
        "local/include"
        "local/include/opus"
    NO_DEFAULT_PATH
)

find_library(OPUS_LIBRARY
    NAMES
        "opus"
    PATHS
        "/usr"
        "/opt"
        "/"
    PATH_SUFFIXES
        "dynamic/lib/opus"
        "lib"
        "opus"
        "local/lib"
        "local/lib/opus"
        "lib/x86_64-linux-gnu"
    NO_DEFAULT_PATH
)


set(OPUS_INCLUDE_DIRS ${OPUS_INCLUDE_DIR})
set(OPUS_LIBRARIES ${OPUS_LIBRARY})

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(Opus
    REQUIRED_VARS OPUS_INCLUDE_DIRS OPUS_LIBRARIES
)
