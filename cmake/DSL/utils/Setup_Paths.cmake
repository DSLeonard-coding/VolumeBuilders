include (${CMAKE_CURRENT_LIST_DIR}/../utils/siphon_user_env.cmake)
set(local_bin "./bin")
set(local_lib "./lib")
siphon_user_env(local_bin)
siphon_user_env(local_lib)

# Convert to absolute paths so build artifacts go to the right place
get_filename_component(INSTALL_PATH "${local_bin}" ABSOLUTE)
get_filename_component(LIB_INSTALL_PATH "${local_lib}" ABSOLUTE)
