# GeoModulesConfig.cmake D.S. Leonard 2026
# Just include("path/to/GeoModulesConfig.cmake")
# and use the resulting GEO_MODULES_SOURCES and GEO_MODULES_INCLUDE_DIR
#
# This is a source library.  The app performs code registration into the library code
# So compilation is handled by the app build.
# But the machinery to do that is provided just by including this file.
# All you need to provide is the modules!
#
# /my_lib/MyLibFiles.cmake

set(LIB_SCRIPT_DIR "${CMAKE_CURRENT_LIST_DIR}")

# Run module code injection (into master X-header) script:
execute_process(
        COMMAND "${LIB_SCRIPT_DIR}/build-autoincludes"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE script_res
)

if(NOT script_res EQUAL 0)
    message(FATAL_ERROR "Auto-include generation failed in ${CMAKE_SOURCE_DIR}")
endif()

#Re-trigger the code injection if any modules change
file(GLOB_RECURSE _ FOLLOW_SYMLINKS CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/src/*.inc
        ${CMAKE_SOURCE_DIR}/include/*.inc
)
file(GLOB _ FOLLOW_SYMLINKS CONFIGURE_DEPENDS
        ${src}/linked/*
)

file(GLOB_RECURSE GEOMODULES_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/*.cc
)

set(GEOMODULES_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
