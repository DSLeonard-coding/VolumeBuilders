# CPM Package Sources: loose refs (branches, tags). Edit here, then run lock-deps.py.

# DSLeonard-cmake
CPMDeclarePackage(DSLeonard-cmake
  NAME DSLeonard-cmake
  GIT_TAG master
  DOWNLOAD_ONLY YES
  GIT_REPOSITORY https://github.com/DSLeonard-coding/DSLeonard-cmake
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/dependencies/DSLeonard-cmake
)
