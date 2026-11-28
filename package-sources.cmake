# CPM Package Sources: loose refs (branches, tags). Edit here, then run lock-deps.py.

# DSLeonard-cmake
CPMDeclarePackage(DSLeonard-cmake
  NAME DSLeonard-cmake
  GIT_TAG master
  DOWNLOAD_ONLY YES
  GIT_REPOSITORY https://github.com/DSLeonard-coding/DSLeonard-cmake
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/dependencies/DSLeonard-cmake
)

# DLG4Units
CPMDeclarePackage(DLG4Units
  NAME DLG4Units
  GIT_TAG master
  DOWNLOAD_ONLY YES
  GIT_REPOSITORY https://github.com/DSLeonard-coding/DLG4Units
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/deps/DLG4Units
)

# DLG4GeoModules
CPMDeclarePackage(DLG4GeoModules
  NAME DLG4GeoModules
  GIT_TAG master
  DOWNLOAD_ONLY YES
  GIT_REPOSITORY https://github.com/DSLeonard-coding/DLG4GeoModules
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/deps/DLG4GeoModules
)

# VBDemo
CPMDeclarePackage(VBDemo
  NAME VBDemo
  GIT_TAG master
  DOWNLOAD_ONLY YES
  GIT_REPOSITORY https://github.com/DSLeonard-coding/VBDemo
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/deps/VBDemo
)
