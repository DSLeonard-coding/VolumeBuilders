#siphon_user_envs D.S.Leonard 2026, MIT
#siphons env variables from a shell script.
macro(siphon_user_envs VAR_LIST_OR_NAME)
    if(NOT DEFINED ${VAR_LIST_OR_NAME})
        set(VAR_LIST ${VAR_LIST_OR_NAME})
    else()
        set(VAR_LIST ${${VAR_LIST_OR_NAME}})
    endif()
    if(EXISTS "${SETUP_SCRIPT}")
        set(_is_optional FALSE)
        foreach(_arg ${ARGN})
            if("${_arg}" STREQUAL "OPTIONAL")
                set(_is_optional TRUE)
            endif()
        endforeach()
        foreach(VAR_NAME ${VAR_LIST})
            execute_process(
                    COMMAND bash -c "source ${SETUP_SCRIPT}> /dev/null 2>&1  ; echo \$${VAR_NAME}"
                    OUTPUT_VARIABLE DETECTED_VALUE
                    OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if(DETECTED_VALUE)
                set(${VAR_NAME} "${DETECTED_VALUE}")
                set(ENV{${VAR_NAME}} "${DETECTED_VALUE}")
                message(STATUS "  [Siphon] ${VAR_NAME} -> ${DETECTED_VALUE}")
            elseif (_is_optional)
                message(STATUS " [Siphon] Did not find OPTIONAL ENV variable ${VAR_NAME}")
            else()
                message(FATAL_ERROR
                        "\n[Siphon FATAL]: Environment variable '${VAR_NAME}' is not set!\n"
                        "This is required for the Geant4 runtime. Check your setup script.\n"
                )
            endif()
        endforeach()
    else()
        message(STATUS "Please copy ${SETUP_FILE} to your home directory and edit the paths in it")
    endif()

endmacro()
