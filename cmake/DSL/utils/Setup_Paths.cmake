#future proofed for vcpkg
if(POLICY CMP0177)
    cmake_policy(SET CMP0177 NEW)
endif()
if(NOT VCPKG_TOOLCHAIN AND NOT CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
    include(${CMAKE_CURRENT_LIST_DIR}/../utils/siphon_user_env.cmake)

    set(local_bin "./bin")
    set(local_lib "./lib")
    siphon_user_envs(local_bin OPTIONAL)
    siphon_user_envs(local_lib OPTIONAL)

    get_filename_component(INSTALL_PATH "${local_bin}" ABSOLUTE)
    get_filename_component(LIB_INSTALL_PATH "${local_lib}" ABSOLUTE)

    # Force these into the cache so the dsl_package_export macro sees them
    set(CMAKE_INSTALL_DATADIR "${INSTALL_PATH}/../share" CACHE PATH "" FORCE)
    set(CMAKE_INSTALL_INCLUDEDIR "${INSTALL_PATH}/../include" CACHE PATH "" FORCE)
    set(CMAKE_INSTALL_LIBDIR "${LIB_INSTALL_PATH}" CACHE PATH "" FORCE)
    set(CMAKE_INSTALL_BINDIR "${INSTALL_PATH}" CACHE PATH "" FORCE)
else()
    message(STATUS "VCPKG detected: Siphon script is yielding to vcpkg paths.")
endif()