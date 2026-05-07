#Modular cern CMake, D.S. Leonard 2026 MIT
#See DSLeonard_cmake subtree repo on DSLeonard's gitlab.
cmake_minimum_required(VERSION 3.15)
message(STATUS "=== VBDemo CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "=== VBDemo CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "=== VBDemo CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

project(VBDemo CXX)
set(base_path ${CMAKE_CURRENT_SOURCE_DIR})

# This GeoModulesConfig.cmake is for VolumeBuilder
# It defines a static library for VolumeBuilder, including its source files,
# its headers, and its necessary Geant4 dependencies.

# Ensure minimum CMake version consistent with your main project
cmake_minimum_required(VERSION 3.15)

# Set C++ standard consistent with your main project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#-----------------------------------------------------------------------------#
#Configure cmake helpers
#-----------------------------------------------------------------------------#
set(SETUP_FILE "vb_user_setup.sh")
set(SETUP_SCRIPT "$ENV{HOME}/${SETUP_FILE}")
set(DSL_CMAKE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/DSL/")

#-----------------------------------------------------------------------------#
#Include helpers and ROOT/Geant builders
#-----------------------------------------------------------------------------#
include("${DSL_CMAKE_PATH}/cern/Geant4_Target.cmake")
include("${DSL_CMAKE_PATH}/cern/ROOT_Target.cmake")
include("${DSL_CMAKE_PATH}/utils/misc.cmake")
include("${DSL_CMAKE_PATH}/utils/Setup_Paths.cmake")

#-----------------------------------------------------------------------------#
#Include the GeoModules source library config
#-----------------------------------------------------------------------------#
include("../GeoModules/GeoModulesConfig.cmake")


#-----------------------------------------------------------------------------#
# Set a default build type if none is specified (e.g., when building from command line)
# This is important for consistent behavior.
#-----------------------------------------------------------------------------#
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified, defaulting to Release.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    # Set the property for the cache entry
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "Debug-ASan")
endif ()
message(STATUS "CMAKE_BUILD_TYPE is: ${CMAKE_BUILD_TYPE}")

#-----------------------------------------------------------------------------#
# Flags and Options
# For C++ flags:
#-----------------------------------------------------------------------------#
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "Flags used by the C++ compiler for profiling builds." FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g" CACHE STRING "Flags used by the C++ compiler for profiling builds." FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Flags used by the C++ compiler for release builds." FORCE)
set(CMAKE_CXX_FLAGS_DEBUG_ASAN "-O3 -g -DUSE_ASAN" CACHE STRING "Flags used by the C++ compiler for release builds." FORCE)

# For C flags
#set(CMAKE_C_FLAGS_RELEASE "-O3 " CACHE STRING "Flags used by the C compiler for release builds." FORCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OPTIONS -DG4VIS_USE)

#-----------------------------------------------------------------------------#
#VOLUMEBUILDERS
#-----------------------------------------------------------------------------#

set(VOLUMEBUILDERS_NO_DEMO ON CACHE BOOL "Disable demo executable build" FORCE)
add_subdirectory(../ VolumeBuilders-build)


#-----------------------------------------------------------------------------#
#VBDemo
#-----------------------------------------------------------------------------#

# --- Source files ---
file(GLOB_RECURSE SRC_FILES FOLLOW_SYMLINKS CONFIGURE_DEPENDS
        ${base_path}/src/*.cc
)
# Remove any files under src/linked/VolumeBuilders from SRC_FILES1
list(FILTER SRC_FILES EXCLUDE REGEX ".*/src/linked/VolumeBuilders/.*")

#Since we are the demo, especially must prevent recurssoin:
set(VOLUMEBUILDERS_NO_DEMO ON CACHE BOOL "Disable demo build from library" FORCE)
message(STATUS "Found source files: ${SRC_FILES}")
add_executable(${PROJECT_NAME} ${SRC_FILES} ${GEOMODULES_SOURCES})

#---- End Source files----------

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin
)
add_custom_target(${PROJECT_NAME}_clion_install_target
        DEPENDS ${PROJECT_NAME}  # Add all targets that must be built before install
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target install --config $<CONFIG>
        COMMENT "Run install target"
)


# --- Include directories (as in Makefile) ---
target_include_directories(${PROJECT_NAME} PRIVATE
        ${base_path}
        ${base_path}/include
        ${GEOMODULES_INCLUDE_DIR}
)

# --- Compile options common to all builds ---
target_compile_options(${PROJECT_NAME} PRIVATE
        -mfpmath=sse
        -fPIC
        ${OPTIONS}
        -DG4USE_STD11
        -DNDEBUG
        -DG4VIS_USE
        -fno-trapping-math
        -ftree-vectorize
        -g
        -fno-math-errno
)

# --- Get git describe ---
execute_process(
        COMMAND git describe --dirty --always
        WORKING_DIRECTORY ${base_path}
        OUTPUT_VARIABLE GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
target_compile_definitions(${PROJECT_NAME} PRIVATE GIT_DESCRIBE="${GIT_DESCRIBE}")


# --- Explicitly find system visualization and GUI libraries ---
# This is crucial. Even if geant4-config lists -lG4OpenGL,
# libG4OpenGL.so has its own dependencies (like Qt5OpenGL, X11 libs)
# that CMake needs to be aware of for correct linking, especially transitively.
find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED) # X11 generally brings in Xext, Xmu, SM, ICE, Xt.
# If not, we might need more specific find_package calls.


# --- Link libraries ---
target_link_libraries(${PROJECT_NAME} PRIVATE
        DLG4::VolumeBuilders
        ROOT_Target
        Geant4_Target
        # Link explicit CMake targets for system libraries.
        # This provides the necessary -L and -l flags, and handles transitive deps.
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::OpenGL
        OpenGL::GL
        X11::X11
        # You may need to add more specific X11 components if X11::X11 is not enough
        # If the Xmu, SM, ICE, Xt find_package calls succeed, link their targets:
        # Xmu::Xmu # Uncomment if find_package(Xmu) creates this target
        # SM::SM   # Uncomment if find_package(SM) creates this target
        # ICE::ICE # Uncomment if find_package(ICE) creates this target
        # Xt::Xt   # Uncomment if find_package(Xt) creates this target
)


# Make sure we use the install RPATH
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)


# Set output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
get_directory_property(hasParent PARENT_DIRECTORY)
if (hasParent)
    # We're being built as part of a larger project
    set_target_properties(${PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
else ()
    # We're being built standalone
    set_target_properties(${PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
    )
    message(STATUS "Stand-alone ${PROJECT_NAME} build")
endif ()

# Set default build type if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif ()





