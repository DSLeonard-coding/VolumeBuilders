#CupSimNoClean.cmake D.S.Leonard 2026
#
#  Builds cupsim if it doesn't exist, but does not clean it on a Clion cache rebuild.
#  Libraries are made in place.
#  Adds an explicit clean-externals target.
#
#This

# Set the actual file paths FIRST (Crucial for add_custom_command syntax) ---
set(MCOBJS_SO "${MCObjs_path}/build/libMCObjs.so")
set(CUPSIM_SO "${CupSim_path}/build/libCupSimL.so")

# Get the absolute path so it doesn't matter who is asking


# Define MCObjs build command
add_custom_command(
        OUTPUT "${MCOBJS_SO}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${MCObjs_path}/build"
        COMMAND ${CMAKE_COMMAND} -S "${MCObjs_path}" -B "${MCObjs_path}/build"
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        "-DROOT_DIR=${ROOT_DIR_PATH}"
        "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
        "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
        COMMAND ${CMAKE_COMMAND} --build "${MCObjs_path}/build" -j2
        COMMENT "Building MCObjs using ROOT at ${ROOT_DIR_PATH}..."
        VERBATIM
)

# Define CupSim build command
add_custom_command(
        OUTPUT "${CUPSIM_SO}"
        DEPENDS "${MCOBJS_SO}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CupSim_path}/build"
        COMMAND ${CMAKE_COMMAND} -S "${CupSim_path}" -B "${CupSim_path}/build"
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        "-DROOT_DIR=${ROOT_DIR_PATH}"
        -DMCObjs="Exist!"
        -DMCObjs_INCLUDE_DIR:PATH="${MCObjs_path}"
        "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
        "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
        COMMAND ${CMAKE_COMMAND} --build "${CupSim_path}/build" -j2
        COMMENT "Building CupSim using ROOT at ${ROOT_DIR_PATH}..."
        VERBATIM
)

# Define IMPORTED targets (Prevents 'Clean All' from deleting .so files) ---
add_library(MCObjs SHARED IMPORTED GLOBAL)
set_target_properties(MCObjs PROPERTIES
        IMPORTED_LOCATION "${MCOBJS_SO}"
        INTERFACE_INCLUDE_DIRECTORIES "${MCObjs_path}"
)

add_library(CupSimL SHARED IMPORTED GLOBAL)
set_target_properties(CupSimL PROPERTIES
        IMPORTED_LOCATION "${CUPSIM_SO}"
        INTERFACE_INCLUDE_DIRECTORIES "${CupSim_path}"
        INTERFACE_LINK_LIBRARIES MCObjs
)

# Create the Glue Target for build dependency ---
add_custom_target(Externals DEPENDS "${MCOBJS_SO}" "${CUPSIM_SO}")

# Initialize or Attach to the Global clean-externals target ---
if(NOT TARGET clean-externals)
    add_custom_target(clean-externals COMMENT "Global Clean for External Libs")
endif()

add_custom_command(
        TARGET clean-externals POST_BUILD
        COMMAND ${CMAKE_COMMAND} --build "${MCObjs_path}/build" --target clean
        COMMAND ${CMAKE_COMMAND} --build "${CupSim_path}/build" --target clean
        COMMENT "Adding CupSim and MCObjs to the global clean-externals task"
)

add_dependencies(MCObjs Externals)
add_dependencies(CupSimL Externals)

