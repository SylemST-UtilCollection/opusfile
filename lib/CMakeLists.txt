cmake_minimum_required(VERSION 3.0)

set(exe_name "opusfile")


##################################
#        Extra definitions       #
##################################

# add_definitions("-std=c++11")

##################################
#       Gather Dependencies      #
##################################

set(LIBRARY
    DruidOpus-Dep
    DruidOgg-Dep
)

##################################
#          Build Binary          #
##################################

FILE(GLOB_RECURSE source_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*
)
FILE(GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h*
)

add_library(${exe_name}
    ${source_files}
    ${header_files}
)

# add_executable(${exe_name} ${source_files} ${header_files})

target_link_libraries(${exe_name}
    ${LIBRARY}
)
target_include_directories(${exe_name}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${INCLUDE}
)

##################################
#       sort out resources       #
##################################

# druid_copy_target_dir(${exe_name} "base" "base")
# druid_append_target_dir(${exe_name} "base" "zip_dir")
