
#       OGG_INCLUDE_DIRS      Header file directories
#       OGG_LIBRARIES         Archive/shared objects
#       OGG_FOUND         Archive/shared objects

find_path(OGG_INCLUDE_DIR
    NAMES
        "ogg/ogg.h"
    PATHS
        "/usr"
        "/opt"
        "/"
    PATH_SUFFIXES
        "dynamic/include/ogg"
        "local"
        "include"
        "external"
        "local/include"
        "ogg"
    NO_DEFAULT_PATH
)

find_library(OGG_LIBRARY
    NAMES
        "ogg"
    PATHS
        "/usr"
        "/opt"
        "/"
    PATH_SUFFIXES
        "dynamic/lib/ogg"
        "lib"
        "ogg"
        "local/lib"
        "lib/x86_64-linux-gnu"
        "local/lib/ogg"
    NO_DEFAULT_PATH
)


set(OGG_INCLUDE_DIRS ${OGG_INCLUDE_DIR})
set(OGG_LIBRARIES ${OGG_LIBRARY})

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(Ogg
    REQUIRED_VARS OGG_INCLUDE_DIRS OGG_LIBRARIES
)
