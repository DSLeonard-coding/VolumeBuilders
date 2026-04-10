macro(dsl_package_export)
    set(oneValueArgs TARGET NAMESPACE)
    set(multiValueArgs DEPS)
    cmake_parse_arguments(DSL "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # 1. Install Targets (The binaries)
    install(TARGETS ${DSL_TARGET}
            EXPORT ${DSL_TARGET}Targets
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    # 2. Install Headers
    # Assumes your headers are in "${CMAKE_CURRENT_SOURCE_DIR}/include"
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    # 3. Generate the Config file (The "Map")
    set(CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/${DSL_TARGET}Config.cmake")

    # Write the header of the config file
    file(WRITE "${CONFIG_FILE}" "include(CMakeFindDependencyMacro)\n")

    # Add dependencies ONLY if they were passed to the macro
    foreach(dep ${DSL_DEPS})
        file(APPEND "${CONFIG_FILE}" "find_dependency(${dep})\n")
    endforeach()

    # Link the Targets file
    file(APPEND "${CONFIG_FILE}" "include(\"\${CMAKE_CURRENT_LIST_DIR}/${DSL_TARGET}Targets.cmake\")\n")



#    # Install the Config and Targets files
#    install(FILES "${CONFIG_FILE}"
#            DESTINATION share/${DSL_TARGET}  # MUST be share/<name>
#    )
#
#    install(EXPORT ${DSL_TARGET}Targets
#            FILE ${DSL_TARGET}Targets.cmake
#            NAMESPACE ${DSL_NAMESPACE}::
#            DESTINATION share/${DSL_TARGET}  # MUST be share/<name>
#    )
#




    # 4. Install the Config and Targets files
    install(FILES "${CONFIG_FILE}"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/${DSL_TARGET}
    )

    install(EXPORT ${DSL_TARGET}Targets
            FILE ${DSL_TARGET}Targets.cmake
            NAMESPACE ${DSL_NAMESPACE}::
            DESTINATION ${CMAKE_INSTALL_DATADIR}/${DSL_TARGET}
    )
endmacro()