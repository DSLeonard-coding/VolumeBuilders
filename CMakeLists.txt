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
if(NOT SETUP_FILE)
    set(SETUP_FILE "vb_user_setup.sh")
endif()
set(SETUP_SCRIPT "$ENV{HOME}/${SETUP_FILE}")
set(DSL_CMAKE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/DSL/")
include("${DSL_CMAKE_PATH}/cern/Geant4_Target.cmake")
include("${DSL_CMAKE_PATH}/cern/ROOT_Target.cmake")
include("${DSL_CMAKE_PATH}/utils/Setup_Paths.cmake")
include("${DSL_CMAKE_PATH}/utils/dsl_package_export.cmake")

add_subdirectory(demo EXCLUDE_FROM_ALL)

if(NOT VOLUMEBUILDERS_NO_DEMO)
    set_property(TARGET VBDemo PROPERTY EXCLUDE_FROM_ALL FALSE)
endif()

# --- Source files for VolumeBuilder ---
file(GLOB_RECURSE VOLUMEBUILDERS_SRC_CC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
file(GLOB_RECURSE VOLUMEBUILDERS_SRC_CPP "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(APPEND VOLUMEBUILDERS_SRC ${VOLUMEBUILDERS_SRC_CC} ${VOLUMEBUILDERS_SRC_CPP})

# --- Define the VolumeBuilder static library ---
add_library(VolumeBuilders STATIC ${VOLUMEBUILDERS_SRC}
        include/BuilderFactories.hh)
add_library(DLG4::VolumeBuilders ALIAS VolumeBuilders)

# --- Include directories for VolumeBuilder ---
# including Geant4
# --- Include directories for VolumeBuilder ---
target_include_directories(VolumeBuilders PUBLIC
        # Use the absolute path ONLY during the build phase
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # Use the relative path once it is installed in vcpkg_installed/
        $<INSTALL_INTERFACE:include>
)



# --- Link Libraries for VolumeBuilder ---
# Link to Geant4 libraries.
target_link_libraries(VolumeBuilders PRIVATE
    ROOT_Target
    Geant4_Target
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

dsl_package_export(
        TARGET VolumeBuilders
        NAMESPACE DLG4
        DEPS ""  # empty for now
)

