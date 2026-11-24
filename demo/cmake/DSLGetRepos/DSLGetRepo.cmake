include_guard(GLOBAL)
#DSLGetRepo.cmake  D.S. Leonard, 2026
#
#Provides one-liner in-tree wrapper to CPM project management, ie dependency repo fetching.
# See github.com/DSLeonard-coding/DSLGetRepos   for documentation.

include("${CMAKE_CURRENT_LIST_DIR}/CPM.cmake")
set(CPM_INCLUDE_ALL_IN_PACKAGE_LOCK ON)
set(CPM_LOCK_FILE "${CMAKE_SOURCE_DIR}/package-lock.cmake" CACHE FILEPATH "CPM lock file")
CPMUsePackageLock(${CPM_LOCK_FILE})

option(DGR_LINK_RELATIVE "Create relative symlinks (survive moving the tree)" ON)

# Sets ${result} (in the caller) to TRUE if `dir` is a working git clone, else FALSE.
function(is_git_repo dir result)
    set(${result} FALSE PARENT_SCOPE)
    if(EXISTS "${dir}/.git")
        # checks if it's really a valid repo.
        execute_process(COMMAND git rev-parse --git-dir
                WORKING_DIRECTORY "${dir}"
                RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET)
        if(rc EQUAL 0)
            set(${result} TRUE PARENT_SCOPE)
        endif()
    endif()
endfunction()


# Add `path` to the .gitignore next to it, if not already there.
# A trailing slash only matches directories, so a symlink gets an entry without one.
function(git_ignore_entry path)
    get_filename_component(parent "${path}" DIRECTORY)
    get_filename_component(name "${path}" NAME)
    set(gitignore "${parent}/.gitignore")

    if(IS_SYMLINK "${path}")
        set(entry "/${name}")
    else()
        set(entry "/${name}/")
    endif()

    set(lines "")
    if(EXISTS "${gitignore}")
        file(STRINGS "${gitignore}" lines)
    endif()
    if(NOT "${entry}" IN_LIST lines)
        file(APPEND "${gitignore}" "${entry}\n")
    endif()
endfunction()


# Make `link` a symlink to `target`. No-op if they are already the same place.
function(make_symlink link target)
    get_filename_component(real_target "${target}" REALPATH)
    get_filename_component(real_link   "${link}"   REALPATH)
    if(real_link STREQUAL real_target)
        return()
    endif()

    if(IS_SYMLINK "${link}")                     # stale or dangling link: replace
        file(REMOVE "${link}")
    elseif(EXISTS "${link}")
        set(children "")
        if(IS_DIRECTORY "${link}")
            file(GLOB children LIST_DIRECTORIES true "${link}/*" "${link}/.*")
        endif()
        if(IS_DIRECTORY "${link}" AND NOT children)
            file(REMOVE_RECURSE "${link}")       # empty dir, e.g. uninitialised submodule
        else()
            message(FATAL_ERROR
                    "dsl_get_repo: cannot symlink ${link} -> ${target}: "
                    "path exists and is not an empty directory or symlink. "
                    "Remove it (or deinit the old submodule) and reconfigure.")
        endif()
    endif()

    get_filename_component(link_parent "${link}" DIRECTORY)
    file(MAKE_DIRECTORY "${link_parent}")

    if(DGR_LINK_RELATIVE)
        file(RELATIVE_PATH link_target "${link_parent}" "${target}")
    else()
        set(link_target "${target}")
    endif()

    file(CREATE_LINK "${link_target}" "${link}" SYMBOLIC RESULT rc)
    if(NOT "${rc}" STREQUAL "0")
        message(FATAL_ERROR
                "dsl_get_repo: failed to create symlink ${link} -> ${link_target}: ${rc}\n"
                "(On Windows this needs Developer Mode or elevated privileges.)")
    endif()
endfunction()


# dsl_repo_impl(NAME REPO TAG DOWNLOAD_ONLY_MODE [DIR] CPM_ARGS)
#
# The real source (the "home") is always ${DGR_REPOS_DIR}/${NAME}, or the user's
# CPM_<NAME>_SOURCE if set.  It is returned in ${NAME}_SOURCE_DIR.
#
# DIR (or, if not given, DGR_<PROJECT_NAME>_REPOS_DIR}/${NAME} when that variable is set)
# is where the calling project wants the repo to appear.  If that is not the home,
# it becomes a symlink to the home.  Relative paths are relative to the calling
# CMakeLists.txt (CMAKE_CURRENT_SOURCE_DIR).
function(_dsl_repo_impl NAME REPO TAG DOWNLOAD_ONLY_MODE)
    # where real sources live: only a cache value (-DDGR_REPOS_DIR=...) can move it
    if(DEFINED CACHE{DGR_REPOS_DIR})
        get_filename_component(store "$CACHE{DGR_REPOS_DIR}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")
    else()
        set(store "${CMAKE_SOURCE_DIR}/dependencies")
    endif()

    # where the real source lives: user override, else the one top-level store
    if(DEFINED CPM_${NAME}_SOURCE)
        set(home "${CPM_${NAME}_SOURCE}")
        set(overridden TRUE)
    else()
        set(home "${store}/${NAME}")
        set(overridden FALSE)
    endif()

    # where the caller wants it to appear (a symlink unless it is the home)
    cmake_parse_arguments(PARSE_ARGV 4 _dgr "" "DIR" "")

    if(DEFINED _dgr_DIR)
        set(wanted "${_dgr_DIR}")
    elseif(DEFINED PROJECT_NAME AND DEFINED DGR_${PROJECT_NAME}_REPOS_DIR)
        set(wanted "${DGR_${PROJECT_NAME}_REPOS_DIR}/${NAME}")
    else()
        set(wanted "${home}")
    endif()

    # relative paths mean relative to the calling CMakeLists.txt
    get_filename_component(home   "${home}"   ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    get_filename_component(wanted "${wanted}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

    # serialize across processes (parallel CLion profiles); released when we return
    file(LOCK "${home}.lock" GUARD FUNCTION TIMEOUT 300 RESULT_VARIABLE locked)
    if(NOT locked EQUAL 0)
        message(FATAL_ERROR "dsl_get_repo: timed out waiting for ${home}.lock")
    endif()

    if(overridden OR EXISTS "${home}/.developing")
        # user-supplied or in-development source: CPM must not touch it
        set(CPM_${NAME}_SOURCE "${home}")
    else()
        # anything else CPM manages, so it updates when the locks (shas) say to
        is_git_repo("${home}" valid_clone)
        if(NOT valid_clone)
            # folder missing or unusable: drop this build dir's stale record, then clone
            string(TOLOWER "${NAME}" lower_name)
            file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/_deps/${lower_name}-subbuild")
        endif()
    endif()

    foreach(arg IN LISTS _dgr_UNPARSED_ARGUMENTS)
        if(arg STREQUAL "SOURCE_DIR" OR arg STREQUAL "DOWNLOAD_ONLY"
                OR arg STREQUAL "NAME" OR arg STREQUAL "GIT_REPOSITORY"
                OR arg STREQUAL "GIT_TAG")
            message(FATAL_ERROR
                    "dsl_get_repo: Direct pass through of CPM ${arg} arg is not supported")
        endif()
    endforeach()

    # STRIP DOWNLOAD only option from locks
    if(DEFINED CPM_DECLARATION_${NAME})
        set(decl "${CPM_DECLARATION_${NAME}}")
        list(FIND decl DOWNLOAD_ONLY idx)
        if(NOT idx EQUAL -1)
            math(EXPR val_idx "${idx} + 1")
            list(REMOVE_AT decl ${idx} ${val_idx})
            set(CPM_DECLARATION_${NAME} "${decl}")
        endif()
    endif()

    CPMAddPackage(
            NAME ${NAME}
            GIT_REPOSITORY ${REPO}
            GIT_TAG ${TAG}
            SOURCE_DIR "${home}"
            DOWNLOAD_ONLY ${DOWNLOAD_ONLY_MODE}
            ${_dgr_UNPARSED_ARGUMENTS})

    if(NOT overridden)
        git_ignore_entry("${home}")
    endif()

    if(NOT wanted STREQUAL home)
        make_symlink("${wanted}" "${home}")
        git_ignore_entry("${wanted}")
    endif()

    # CPMAddPackage's results landed in this function's scope; pass them on to the caller
    foreach(var SOURCE_DIR BINARY_DIR ADDED)
        set(${NAME}_${var} "${${NAME}_${var}}" PARENT_SCOPE)
    endforeach()
    set(CPM_LAST_PACKAGE_NAME "${CPM_LAST_PACKAGE_NAME}" PARENT_SCOPE)
endfunction()

# DOWNLOAD_ONLY mode)
function(dsl_get_repo NAME REPO TAG)
    _dsl_repo_impl("${NAME}" "${REPO}" "${TAG}" YES ${ARGN})
    foreach(var SOURCE_DIR BINARY_DIR ADDED)
        set(${NAME}_${var} "${${NAME}_${var}}" PARENT_SCOPE)
    endforeach()
    set(CPM_LAST_PACKAGE_NAME "${CPM_LAST_PACKAGE_NAME}" PARENT_SCOPE)
endfunction()

# Fetch and add targets into the build (no DOWNLOAD_ONLY YES)
function(dsl_add_repo NAME REPO TAG)
    _dsl_repo_impl("${NAME}" "${REPO}" "${TAG}" NO ${ARGN})
    foreach(var SOURCE_DIR BINARY_DIR ADDED)
        set(${NAME}_${var} "${${NAME}_${var}}" PARENT_SCOPE)
    endforeach()
    set(CPM_LAST_PACKAGE_NAME "${CPM_LAST_PACKAGE_NAME}" PARENT_SCOPE)
endfunction()