# This CMakeLists.txt is for VolumeBuilder
# It defines a static library for VolumeBuilder, including its source files,
# its headers, and its necessary Geant4 dependencies.

# Ensure minimum CMake version consistent with your main project
cmake_minimum_required(VERSION 3.15)

# Define a project for this sub-component
project(VolumeBuilders LANGUAGES CXX)

# Set C++ standard consistent with your main project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(demo)

execute_process(
        COMMAND $ENV{GEANT4_PATH}/bin/geant4-config --cflags
        OUTPUT_VARIABLE GEANT4_CONFIG_CFLAGS_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
separate_arguments(GEANT4_CONFIG_CFLAGS_RAW)

set(GEANT4_INCLUDE_DIRS "")
set(GEANT4_DEFINITIONS "")

foreach(flag ${GEANT4_CONFIG_CFLAGS_RAW})
    if(flag MATCHES "^-I(.+)")
        list(APPEND GEANT4_INCLUDE_DIRS "${CMAKE_MATCH_1}")
    elseif(flag MATCHES "^-D(.+)")
        list(APPEND GEANT4_DEFINITIONS "${CMAKE_MATCH_1}")
    endif()
endforeach()

execute_process(
        COMMAND $ENV{GEANT4_PATH}/bin/geant4-config --libs
        OUTPUT_VARIABLE GEANT4_CONFIG_LIBS_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
separate_arguments(GEANT4_CONFIG_LIBS_RAW)

# --- Source files for VolumeBuilder ---
file(GLOB_RECURSE VOLUMEBUILDERS_SRC_CC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
file(GLOB_RECURSE VOLUMEBUILDERS_SRC_CPP "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(APPEND VOLUMEBUILDERS_SRC ${VOLUMEBUILDERS_SRC_CC} ${VOLUMEBUILDERS_SRC_CPP})

# --- Define the VolumeBuilder static library ---
add_library(VolumeBuilders STATIC ${VOLUMEBUILDERS_SRC})
add_library(DLG4::VolumeBuilders ALIAS VolumeBuilders)

# --- Include directories for VolumeBuilder ---
# including Geant4
target_include_directories(VolumeBuilders PUBLIC
    # Local includes for VolumeBuilder
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    # Geant4 includes, extracted using geant4-config
    ${GEANT4_INCLUDE_DIRS}
)

# --- Compile Definitions for VolumeBuilder ---
# Apply any Geant4-related definitions if geant4-config provides them.
target_compile_definitions(VolumeBuilders PUBLIC
    ${GEANT4_DEFINITIONS}
)

# --- Link Libraries for VolumeBuilder ---
# Link to Geant4 libraries.
target_link_libraries(VolumeBuilders PUBLIC
    ${GEANT4_CONFIG_LIBS_RAW}
)

target_compile_options(VolumeBuilders PRIVATE
        -mfpmath=sse
        -fPIC
#        ${OPTIONS}
        -fno-trapping-math
        -ftree-vectorize
        -O0
        -g
        -fno-math-errno
)

target_compile_options(VolumeBuilders PRIVATE
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O0 -DNDEBUG>
)

