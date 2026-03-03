#Modular cern CMake, D.S. Leonard 2026 MIT
#See DSLeonard_cmake subtree repo on DSLeonard's gitlab.
cmake_minimum_required(VERSION 3.15)
message(STATUS "=== VBDemo CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "=== VBDemo CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "=== VBDemo CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

project(VBDemo CXX)

set(SETUP_FILE "vb_user_setup.sh")
set(SETUP_SCRIPT "$ENV{HOME}/${SETUP_FILE}")
set(DSL_CMAKE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/DSL/")
include("${DSL_CMAKE_PATH}/cern/Geant4_Target.cmake")
include("${DSL_CMAKE_PATH}/cern/ROOT_Target.cmake")
include("${DSL_CMAKE_PATH}/utils/Setup_Paths.cmake")

# Set a default build type if none is specified (e.g., when building from command line)
# This is important for consistent behavior.
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified, defaulting to Release.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    # Set the property for the cache entry
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif()
message(STATUS "CMAKE_BUILD_TYPE is: ${CMAKE_BUILD_TYPE}")

# For C++ flags:
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "Flags used by the C++ compiler for profiling builds." FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g" CACHE STRING "Flags used by the C++ compiler for profiling builds." FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Flags used by the C++ compiler for release builds." FORCE)

# For C flags
#set(CMAKE_C_FLAGS_RELEASE "-O3 " CACHE STRING "Flags used by the C compiler for release builds." FORCE)
#...


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OPTIONS -DG4VIS_USE )


# --- Source files ---
file(GLOB_RECURSE SRC_FILES1 FOLLOW_SYMLINKS ${base_path}/src/*.cc)

set(SRC_FILES ${SRC_FILES1})

message(STATUS "Found source files: ${SRC_FILES}")
add_executable(${PROJECT_NAME} ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)

# --- Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${base_path}
        ${base_path}/include
        ${base_path}/../include
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
        -O0
        -g
        -fno-math-errno
)

#This is done with CMAKE_CXX_FLAGS_<BUILD_TYPE> now:
#target_compile_options(${PROJECT_NAME} PRIVATE
#        $<$<CONFIG:Debug>:-g -O0>
#        $<$<CONFIG:Release>:-O0 -DNDEBUG>
#)

# --- Get git describe ---
execute_process(
        COMMAND git describe --dirty --always
        WORKING_DIRECTORY ${base_path}
        OUTPUT_VARIABLE GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
target_compile_definitions(${PROJECT_NAME} PRIVATE GIT_DESCRIBE="${GIT_DESCRIBE}")






# --- Explicitly find system visualization and GUI libraries ---
# Even if geant4-config lists -lG4OpenGL,
# libG4OpenGL.so has its own dependencies (like Qt5OpenGL, X11 libs)
# that CMake needs to be aware of for correct linking, especially transitively.
find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED) # X11 generally brings in Xext, Xmu, SM, ICE, Xt.
# If not, we might need more specific find_package calls.


# --- Link libraries ---
target_link_libraries(${PROJECT_NAME} PRIVATE
        VolumeBuilders
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


# --- Probably Obsolete:
target_link_options(${PROJECT_NAME} PRIVATE
        -Wl,-rpath,${ROOTSYS}/lib
        -Wl,-rpath,${GEANT4_PATH}/lib
)

# Custom target for autoincludes script
add_custom_target(${PROJECT_NAME}_autoincludes
        COMMAND bash ${base_path}/build-autoincludes
        WORKING_DIRECTORY ${base_path}
        COMMENT "Running build-autoincludes script"
)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_autoincludes)

# Set output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "${ROOTSYS}/lib;${GEANT4_PATH}/lib"
        INSTALL_RPATH_USE_LINK_PATH TRUE
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
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
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

