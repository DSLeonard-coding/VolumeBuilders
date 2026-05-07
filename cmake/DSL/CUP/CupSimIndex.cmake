#CupSim.cmake D.S.Leonard 2026

# This does nothing for the build, but forces CLion to index the files
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    file(GLOB_RECURSE EXTERNAL_INDEXING_FILES
            "${MCObjs_path}/*.cc" "${MCObjs_path}/*.hh"
            "${CupSim_path}/*.cc" "${CupSim_path}/*.hh"
    )
    add_custom_target(ExternalIndexingSource SOURCES ${EXTERNAL_INDEXING_FILES})
endif()