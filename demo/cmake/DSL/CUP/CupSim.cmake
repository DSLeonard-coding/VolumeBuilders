
#-----------------------------------------------------------------------------#
#   CUPSIM
#-----------------------------------------------------------------------------#
#Make sure these use absolute paths
add_subdirectory(${MCObjs_path} MCObjs-build)

set(MCObjs "Exist!" CACHE INTERNAL "")
set(MCObjs_INCLUDE_DIR ${MCObjs_path} CACHE INTERNAL "")

add_subdirectory(${CupSim_path} CupSim-build)
set_target_properties(CupSimL PROPERTIES INTERFACE_LINK_LIBRARIES MCObjs)

target_include_directories(MCObjs INTERFACE ${MCObjs_path})
target_include_directories(CupSimL INTERFACE ${CupSim_path})

set_target_properties(MCObjs PROPERTIES INSTALL_RPATH "${CMAKE_BINARY_DIR}/MCObjs-build")
set_target_properties(CupSimL PROPERTIES INSTALL_RPATH "${CMAKE_BINARY_DIR}/CupSim-build")