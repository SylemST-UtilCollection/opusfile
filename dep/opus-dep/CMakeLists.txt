
set(exe_name "DruidOpus-Dep")


##################################
#        Extra definitions       #
##################################


add_definitions("-std=c++11")


##################################
#       Gather Dependencies      #
##################################


unset(LIBRARY)
unset(INCLUDE)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
FIND_PACKAGE(Opus REQUIRED)

if (${OPUS_FOUND})
    list(APPEND INCLUDE ${OPUS_INCLUDE_DIRS})
    list(APPEND LIBRARY ${OPUS_LIBRARIES})
    message("-- Found Opus")
    message("--\t includes: ${INCLUDE}")
    message("--\t libs: ${LIBRARY}")
else ()
    message(FATAL_ERROR "## Not Found - Opus")
endif ()


##################################
#          Build Binary          #
##################################

FILE(GLOB_RECURSE source_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*
)
FILE(GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h*
)

add_library(${exe_name}
    STATIC
    ${source_files}
    ${header_files}
)

# add_executable(${exe_name} ${source_files} ${header_files})
# add_test(${exe_name} ${exe_name})

target_link_libraries(${exe_name}
    ${LIBRARY}
)
target_include_directories(${exe_name}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${INCLUDE}
)

##################################
#       sort out resources       #
##################################
