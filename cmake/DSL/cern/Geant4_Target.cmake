if(TARGET Geant4_Target)
    return()
endif()

include (${CMAKE_CURRENT_LIST_DIR}/../utils/siphon_user_env.cmake)
set(REQUIRED_ENVS
        GEANT4_PATH
        G4ENSDFSTATEDATA
        G4LEDATA
        G4LEVELGAMMADATA
        G4NEUTRONHPDATA
)

siphon_user_envs(REQUIRED_ENVS REQUIRED)

if(NOT GEANT4_PATH)
    message(FATAL_ERROR "GEANT4_PATH not set in Environment or vb_user_setup.sh")
endif()


# This doesn't work for gcc 13 and geant 10.7
#if(G4_CMAKE_FILES)
#    # Take the first match found
#    list(GET G4_CMAKE_FILES 0 G4_FIRST_MATCH)
#    get_filename_component(Geant4_DIR "${G4_FIRST_MATCH}" DIRECTORY)
#    find_package(Geant4 REQUIRED)
#    message(STATUS "Found Geant4 CMake at: ${Geant4_DIR}")
#else()
#    message(FATAL_ERROR "Could not find Geant4Config.cmake in ${GEANT4_PATH}")
#endif()

# --- Hunt for the directory containing Geant4Config.cmake ---
file(GLOB_RECURSE G4_CMAKE_FILES
        "${GEANT4_PATH}/*Geant4Config.cmake")

# Get Geant4 flags old-fashioned way
execute_process(COMMAND ${GEANT4_PATH}/bin/geant4-config --cflags OUTPUT_VARIABLE G4_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${GEANT4_PATH}/bin/geant4-config --libs OUTPUT_VARIABLE G4_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${GEANT4_PATH}/bin/geant4-config --version OUTPUT_VARIABLE Geant4_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${GEANT4_PATH}/bin/geant4-config --prefix OUTPUT_VARIABLE Geant4_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)

# Create the Shadow Target
add_library(Geant4_Target INTERFACE IMPORTED GLOBAL)

separate_arguments(G4_CFLAGS_LIST NATIVE_COMMAND "${G4_CFLAGS}")
foreach(flag ${G4_CFLAGS_LIST})
    if(flag MATCHES "^-I(.+)")
        target_include_directories(Geant4_Target INTERFACE "${CMAKE_MATCH_1}")
    elseif(flag MATCHES "^-D(.+)")
        target_compile_definitions(Geant4_Target INTERFACE "${CMAKE_MATCH_1}")
    endif()
endforeach()

# Clean up the libs string and link it
separate_arguments(G4_LIBS_LIST NATIVE_COMMAND "${G4_LIBS}")
target_link_libraries(Geant4_Target INTERFACE ${G4_LIBS_LIST})


# Extract the library directory for RPATH/Linker use
foreach(flag ${G4_LIBS_LIST})
    if(flag MATCHES "^-L(.+)")
        list(APPEND Geant4_LIB_DIR "${CMAKE_MATCH_1}")
#        set(Geant4_LIB_DIR "${CMAKE_MATCH_1}")
        target_link_directories(Geant4_Target INTERFACE "${Geant4_LIB_DIR}")
    endif()
endforeach()

target_link_libraries(Geant4_Target INTERFACE ${G4_LIBS_LIST})

# RPATH fix for CLion/Transitivity ---
# This ensures that anything linking to Geant4_Target gets the RPATH fix automatically
target_link_options(Geant4_Target INTERFACE "-Wl,--disable-new-dtags")

# Set other standard variables.
get_target_property(Geant4_INCLUDE_DIRS Geant4_Target INTERFACE_INCLUDE_DIRECTORIES)

set(Geant4_LIBRARIES Geant4_Target) # Standard practice: point the variable to the target
set(Geant4_FOUND TRUE)

#Get the dataset info (To get the variable names)
execute_process(COMMAND ${GEANT4_PATH}/bin/geant4-config --datasets
        OUTPUT_VARIABLE G4_DATA_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE)

separate_arguments(G4_DATA_LIST NATIVE_COMMAND "${G4_DATA_RAW}")

# Loop through the triplets
while(G4_DATA_LIST)
    list(GET G4_DATA_LIST 0 G4_NAME)
    list(GET G4_DATA_LIST 1 G4_VAR)
    list(GET G4_DATA_LIST 2 G4_PATH)  # This is the /opt/geant4/... path you verified

    # Set the Environment Variable so CLion/Runtime can see it
    set(ENV{${G4_VAR}} "${G4_PATH}")

    # Export it for CMake use
    set(${G4_VAR} "${G4_PATH}" CACHE INTERNAL "")

    list(REMOVE_AT G4_DATA_LIST 0 1 2)
endwhile()