if(TARGET ROOT_Target)
    return()
endif()

include (${CMAKE_CURRENT_LIST_DIR}/../utils/siphon_user_env.cmake)
siphon_user_envs(ROOTSYS)
if("$ENV{ROOTSYS}" STREQUAL "")
    message(FATAL_ERROR "ROOTSYS not set in Environment or hpge_user_setup.sh")
endif()

set(ROOT_BASE_DIR "$ENV{ROOTSYS}")
# --- 2. Hunt for the directory containing ROOTConfig.cmake ---
file(GLOB_RECURSE ROOT_CMAKE_FILES
        "${ROOT_BASE_DIR}/*ROOTConfig.cmake")

if(ROOT_CMAKE_FILES)
    # Take the first match found to avoid list pollution
    list(GET ROOT_CMAKE_FILES 0 FIRST_MATCH)
    get_filename_component(ROOT_DIR_PATH "${FIRST_MATCH}" DIRECTORY)

    set(ROOT_DIR "${ROOT_DIR_PATH}") # find_package looks for ROOT_DIR
    find_package(ROOT REQUIRED)
    message(STATUS "Found ROOT CMake at: ${ROOT_DIR_PATH}")
else()
    message(FATAL_ERROR "Could not find ROOTConfig.cmake in ${ROOT_BASE_DIR}")
endif()

# Index ROOT source files if they exist
# This target does nothing during build, but forces CLion etc to index the files
if(EXISTS "$ENV{ROOTSYS}/src")
    file(GLOB_RECURSE ROOT_SOURCES "$ENV{ROOTSYS}/src/*.cxx")
    add_custom_target(root_sources_indexer SOURCES ${ROOT_SOURCES})
endif()

add_library(ROOT_Target INTERFACE IMPORTED GLOBAL)
target_include_directories(ROOT_Target INTERFACE ${ROOT_INCLUDE_DIRS})
target_link_libraries(ROOT_Target INTERFACE ${ROOT_LIBRARIES})
# More precision way:
#target_link_libraries(ROOT_Target INTERFACE
#        ROOT::Core
#        ROOT::RIO
#        ROOT::Net
#        ROOT::Hist
#        ROOT::Tree
#)
