cmake_minimum_required(VERSION 3.15)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, e.g. -DCMAKE_BUILD_TYPE= Debug or Release or maybe FORCE")
endif()
message(STATUS "CMAKE_BUILD_TYPE is: ${CMAKE_BUILD_TYPE}")

project(Demo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OPTIONS -DG4VIS_USE )

# --- Directory variables as in Makefile ---
set(Demo_path ${CMAKE_CURRENT_SOURCE_DIR})
set(base_path ${Demo_path})
#add_subdirectory(../ ../build)

# --- Source files ---
file(GLOB_RECURSE SRC_FILES1 FOLLOW_SYMLINKS ${base_path}/src/*.cc)
#file(GLOB_RECURSE SRC_FILES2 FOLLOW_SYMLINKS ${base_path}/../subtrees/VolumeBuilder/src/*.cc)
#file(GLOB_RECURSE SRC_FILES2 FOLLOW_SYMLINKS)
#set(SRC_FILES ${SRC_FILES1} ${SRC_FILES2})
set(SRC_FILES ${SRC_FILES1})
#list(FILTER SRC_FILES EXCLUDE REGEX ".*/linked/.*")
message(STATUS "Found source files: ${SRC_FILES}")
add_executable(Demo ${SRC_FILES})
set_target_properties(Demo PROPERTIES EXCLUDE_FROM_ALL TRUE)

install(TARGETS Demo
        RUNTIME DESTINATION ${Demo_path}/bin
)

# Set environment variables for ROOT and Geant4 paths
set(GEANT4_PATH "$ENV{GEANT4_PATH}")
set(ROOTSYS "$ENV{ROOTSYS}")


# --- Include directories (as in Makefile) ---
target_include_directories(Demo PRIVATE
        ${base_path}
        ${base_path}/include
        ${base_path}/../include
)

# --- Compile options common to all builds ---
target_compile_options(Demo PRIVATE
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

target_compile_options(Demo PRIVATE
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O0 -DNDEBUG>
)
# --- Get git describe ---
execute_process(
        COMMAND git describe --dirty --always
        WORKING_DIRECTORY ${base_path}
        OUTPUT_VARIABLE GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
target_compile_definitions(Demo PRIVATE GIT_DESCRIBE="${GIT_DESCRIBE}")


# --- Get ROOT and Geant4 cflags and parse them ---
execute_process(
        COMMAND ${ROOTSYS}/bin/root-config --cflags
        OUTPUT_VARIABLE ROOT_CONFIG_CFLAGS_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
separate_arguments(ROOT_CONFIG_CFLAGS_RAW)
message(STATUS "ROOT CFLAGS RAW: ${ROOT_CONFIG_CFLAGS_RAW}")

execute_process(
        COMMAND ${GEANT4_PATH}/bin/geant4-config --cflags
        OUTPUT_VARIABLE GEANT4_CONFIG_CFLAGS_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "GEANT4 CFLAGS RAW: ${GEANT4_CONFIG_CFLAGS_RAW}")
separate_arguments(GEANT4_CONFIG_CFLAGS_RAW)

# Extract -I and -D flags from ROOT and Geant4 config outputs
set(CONFIG_INCLUDE_DIRS "")
set(CONFIG_DEFINITIONS "")

foreach(flag ${ROOT_CONFIG_CFLAGS_RAW} ${GEANT4_CONFIG_CFLAGS_RAW})
    if(flag MATCHES "^-I(.+)")
        list(APPEND CONFIG_INCLUDE_DIRS "${CMAKE_MATCH_1}")
    elseif(flag MATCHES "^-D(.+)")
        list(APPEND CONFIG_DEFINITIONS "${CMAKE_MATCH_1}")
    endif()
endforeach()

# Add extracted include directories and compile definitions
target_include_directories(Demo PRIVATE ${CONFIG_INCLUDE_DIRS})
target_compile_definitions(Demo PRIVATE ${CONFIG_DEFINITIONS})


# --- Get ROOT and Geant4 linker flags ---
execute_process(
        COMMAND $ENV{ROOTSYS}/bin/root-config --libs
        OUTPUT_VARIABLE ROOT_CONFIG_LIBS_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
separate_arguments(ROOT_CONFIG_LIBS_RAW)
message(STATUS "ROOT LIBS RAW: ${ROOT_CONFIG_LIBS_RAW}")

execute_process(
        COMMAND $ENV{GEANT4_PATH}/bin/geant4-config --libs
        OUTPUT_VARIABLE GEANT4_CONFIG_LIBS_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
separate_arguments(GEANT4_CONFIG_LIBS_RAW)
message(STATUS "GEANT4 LIBS RAW: ${GEANT4_CONFIG_LIBS_RAW}")


# --- Explicitly find system visualization and GUI libraries ---
# This is crucial. Even if geant4-config lists -lG4OpenGL,
# libG4OpenGL.so has its own dependencies (like Qt5OpenGL, X11 libs)
# that CMake needs to be aware of for correct linking, especially transitively.
find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED) # X11 generally brings in Xext, Xmu, SM, ICE, Xt.
# If not, we might need more specific find_package calls.


# --- Link libraries ---
target_link_libraries(Demo PRIVATE
        VolumeBuilders
        ${ROOT_CONFIG_LIBS_RAW}
        ${GEANT4_CONFIG_LIBS_RAW}
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


# --- Additional linker flags (rpath as in Makefile) ---
target_link_options(Demo PRIVATE
        -Wl,-rpath,$ENV{ROOTSYS}/lib
        -Wl,-rpath,$ENV{GEANT4_PATH}/lib
)

# Custom target for autoincludes script
add_custom_target(autoincludes
        COMMAND bash ${base_path}/build-autoincludes
        WORKING_DIRECTORY ${base_path}
        COMMENT "Running build-autoincludes script"
)
add_dependencies(Demo autoincludes)

# Set output directory for the executable
set_target_properties(Demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Set RPATH for installation and build tree
set(CMAKE_INSTALL_RPATH
        "${ROOTSYS}/lib"
        "${GEANT4_PATH}/lib"
)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)