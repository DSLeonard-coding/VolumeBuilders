macro(siphon_user_env)
    if(EXISTS "${SETUP_SCRIPT}")
        foreach(VAR_NAME ${ARGN})
            execute_process(
                    COMMAND bash -c "source ${SETUP_SCRIPT}> /dev/null 2>&1  ; echo \$${VAR_NAME}"
                    OUTPUT_VARIABLE DETECTED_VALUE
                    OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if(DETECTED_VALUE)
                set(${VAR_NAME} "${DETECTED_VALUE}")
                set(ENV{${VAR_NAME}} "${DETECTED_VALUE}")
                message(STATUS "  [Siphon] ${VAR_NAME} -> ${DETECTED_VALUE}")
            else()
                message(STATUS " [Siphon] Failed to retrieve ENV variable ${VAR_NAME}")
                message(STATUS " [Siphon]       This may or may not be fatal.")
            endif()
        endforeach()
    else()
        message(STATUS "Please copy ${SETUP_FILE} to your home directory and edit the paths in it")
    endif()
endmacro()

macro(siphon_user_envs REQUIRED_LIST IS_REQUIRED)
    foreach(VAR ${${REQUIRED_LIST}})
        # 1. Attempt to pull from the setup script via your existing siphoner
        siphon_user_env(${VAR})

        # 2. Check if it's still empty (both in CMake and actual ENV)
        if(NOT ${VAR} AND NOT ENV{${VAR}})
            if("${IS_REQUIRED}" STREQUAL "REQUIRED")
                message(FATAL_ERROR
                        "\n[Siphon FATAL]: Environment variable '${VAR}' is not set!\n"
                        "This is required for the Geant4 runtime. Check your setup script.\n"
                )
            endif()
        endif()
    endforeach()
endmacro()
