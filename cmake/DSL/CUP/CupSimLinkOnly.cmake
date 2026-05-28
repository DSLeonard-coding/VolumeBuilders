#CupSimLinkOnly.cmake D.S. Leoanrd 2026
# Just link the to the libraries and headers in this version, no building

#-----------------------------------------------------------------------------#
#   CUPSIM
#-----------------------------------------------------------------------------#
#Make sure these use absolute paths

get_filename_component(MCOBJS_LIB_PATH "${MCObjs_path}/lib/libMCObjs.so" ABSOLUTE)
get_filename_component(CUPSIM_LIB_PATH "${CupSim_path}/lib/libCupSimL.so" ABSOLUTE)

# Debug output
message(STATUS "Absolute MCObjs lib path: ${MCOBJS_LIB_PATH}")
message(STATUS "Absolute CupSim lib path: ${CUPSIM_LIB_PATH}")

add_library(MCObjs SHARED IMPORTED)
set_target_properties(MCObjs PROPERTIES
        IMPORTED_LOCATION "${MCOBJS_LIB_PATH}"
        INTERFACE_INCLUDE_DIRECTORIES "${MCObjs_path}/MCObjs"
)

add_library(CupSimL SHARED IMPORTED)
set_target_properties(CupSimL PROPERTIES
        IMPORTED_LOCATION "${CUPSIM_LIB_PATH}"
        INTERFACE_INCLUDE_DIRECTORIES "${CupSim_path}/CupSim"
)

# link_directories is generally discouraged, but used here for consistency with Makefile local paths.
link_directories(
        ${MCObjs_path}/..
        ${CupSim_path}/..
)
# Get absolute paths for the library directories
get_filename_component(MCOBJS_LIB_DIR "${MCObjs_path}/lib" ABSOLUTE)
get_filename_component(CUPSIM_LIB_DIR "${CupSim_path}/lib" ABSOLUTE)


