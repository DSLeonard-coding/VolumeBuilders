# Add ASan only if a specific flag is passed or in a specific build mode
if(USE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

#-----------------------------------------------------------------------------------#
#Custom paths to search for packages:
#-----------------------------------------------------------------------------------#
set(OPTIONAL_ENVS
        CUSTOM_CMAKE_PREFIX_PATHS
)

if(USER_CUSTOM_PATHS)
siphon_user_envs(OPTIONAL_ENVS "OPTIONAL")
    file(TO_CMAKE_PATH "${CUSTOM_CMAKE_PREFIX_PATHS}" PROCESSED_PATHS)
    list(APPEND CMAKE_PREFIX_PATH ${PROCESSED_PATHS})
endif()