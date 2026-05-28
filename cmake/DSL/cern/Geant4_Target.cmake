if(TARGET Geant4_Target)
    return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../utils/siphon_user_env.cmake)

# 1. Bootstrap GEANT4_PATH (Still strict, Siphon will fail if missing)
set(BOOTSTRAP_ENVS GEANT4_PATH)
siphon_user_envs(BOOTSTRAP_ENVS)

# 2. Find Geant4 package
list(APPEND CMAKE_PREFIX_PATH "${GEANT4_PATH}")
find_package(Geant4 REQUIRED OPTIONAL_COMPONENTS
        ui_all
        vis_all
        multithreaded
        gdml
)

# 3. Define dataset variables we care about
set(GEANT4_DATA_VARS
        G4ENSDFSTATEDATA
        G4LEDATA
        G4LEVELGAMMADATA
        G4NEUTRONHPDATA
)

# 4. Create a temporary list for Siphon to check as "Optional" overrides.
# If your Siphon script doesn't have an optional mode, we fake it by creating
# a secondary list that we evaluate carefully, or we let a custom wrapper handle it.
set(TRY_SIPHON ${GEANT4_DATA_VARS})

# If your siphon script ALWAYS fatals on missing variables, we can't pass them to it
# unless they are already present. Instead, we manually check the ENV using CMake,
# mimicking Siphon's priority rules:
foreach(ds_var ${GEANT4_DATA_VARS})
    if(DEFINED ENV{${ds_var}})
        # User has it in their actual shell -> Prioritize it!
        set(${ds_var} "$ENV{${ds_var}}")
    endif()
endforeach()

# 5. Fallback to Geant4's internal locations if the user didn't provide an override
if(Geant4_FOUND)
    if(DEFINED Geant4_DATASETS)
        # --- Geant4 11 Path ---
        foreach(ds ${Geant4_DATASETS})
            set(ds_var "${Geant4_DATASET_${ds}_ENVVAR}")
            # Only use Geant4's path if the shell didn't explicitly override it
            if(NOT DEFINED ${ds_var} OR "${${ds_var}}" STREQUAL "")
                set(${ds_var} "${Geant4_DATASET_${ds}_PATH}")
            endif()
        endforeach()
    elseif(DEFINED Geant4_DATASET_VARIABLES)
        # --- Geant4 10 Legacy Path ---
        foreach(ds_var ${Geant4_DATASET_VARIABLES})
            if(NOT DEFINED ${ds_var} OR "${${ds_var}}" STREQUAL "")
                set(${ds_var} "${${ds_var}}")
            endif()
        endforeach()
    endif()
endif()

# 6. Strict Validation Check (Bypassing Siphon for datasets)
# Since Siphon would fail, we do a manual sanity check. If both the user shell
# AND Geant4 failed to provide a path, then we throw the error.
foreach(ds_var ${GEANT4_DATA_VARS})
    if(NOT DEFINED ${ds_var} OR "${${ds_var}}" STREQUAL "")
        message(FATAL_ERROR "Required Geant4 environment variable missing: ${ds_var}")
    endif()

    # Export securely to Cache and Child ENV for compilation tools
    set(ENV{${ds_var}} "${${ds_var}}")
    set(${ds_var} "${${ds_var}}" CACHE INTERNAL "Geant4 Data Path for ${ds_var}")
endforeach()


# 7. Setup targets (Geant4 11 modern target fallback)
if(Geant4_USE_FILE AND EXISTS "${Geant4_USE_FILE}")
    include(${Geant4_USE_FILE})
endif()

add_library(Geant4_Target INTERFACE IMPORTED GLOBAL)
if(TARGET Geant4::Geant4)
    target_link_libraries(Geant4_Target INTERFACE Geant4::Geant4)
else()
    target_include_directories(Geant4_Target INTERFACE ${Geant4_INCLUDE_DIRS})
    target_compile_definitions(Geant4_Target INTERFACE ${Geant4_DEFINITIONS})
    target_link_libraries(Geant4_Target INTERFACE ${Geant4_LIBRARIES})
endif()

set(Geant4_LIBRARIES Geant4_Target)
set(Geant4_FOUND TRUE)
target_link_options(Geant4_Target INTERFACE "-Wl,--disable-new-dtags")