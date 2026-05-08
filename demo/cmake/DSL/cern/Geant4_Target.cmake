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

siphon_user_envs(REQUIRED_ENVS)

list(APPEND CMAKE_PREFIX_PATH "${GEANT4_PATH}")

find_package(Geant4 REQUIRED OPTIONAL_COMPONENTS
        ui_all
        vis_all
        multithreaded
        gdml
)

# Set up all the compiler flags and definitions
include(${Geant4_USE_FILE})

add_library(Geant4_Target INTERFACE IMPORTED GLOBAL)

target_include_directories(Geant4_Target INTERFACE ${Geant4_INCLUDE_DIRS})
target_compile_definitions(Geant4_Target INTERFACE ${Geant4_DEFINITIONS})
target_link_libraries(Geant4_Target INTERFACE ${Geant4_LIBRARIES})

# Standard practice variables
set(Geant4_LIBRARIES Geant4_Target)
set(Geant4_FOUND TRUE)

# find_package(Geant4) automatically populates Geant4_DATASETS_VARIABLES
foreach(ds_var ${Geant4_DATASET_VARIABLES})
    set(ENV{${ds_var}} "${${ds_var}}")
    set(${ds_var} "${${ds_var}}" CACHE INTERNAL "")
endforeach()

## RPATH fix for CLion/Transitivity ---
## This ensures that anything linking to Geant4_Target gets the RPATH fix automatically
target_link_options(Geant4_Target INTERFACE "-Wl,--disable-new-dtags")


