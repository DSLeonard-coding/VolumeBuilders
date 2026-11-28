include_guard(GLOBAL)
#DSLGetRepo.cmake  D.S. Leonard, 2026
#
#Provides one-liner in-tree wrapper to CPM project management, ie dependency repo fetching.
# See github.com/DSLeonard-coding/DSLGetRepos   for documentation.

#a better guard that prevents other versions from loading in subprojects:
get_property(_dgr_already_loaded GLOBAL PROPERTY DSL_GET_REPO_LOADED)
if(_dgr_already_loaded)
    return()
endif()
set_property(GLOBAL PROPERTY DSL_GET_REPO_LOADED TRUE)

include("${CMAKE_CURRENT_LIST_DIR}/CPM.cmake")
set(CPM_INCLUDE_ALL_IN_PACKAGE_LOCK ON)
set(CPM_LOCK_FILE "${CMAKE_SOURCE_DIR}/package-lock.cmake" CACHE FILEPATH "CPM lock file")
CPMUsePackageLock(${CPM_LOCK_FILE})

option(DGR_LINK_RELATIVE "Create relative symlinks (survive moving the tree)" ON)

function(_dgr_strip_all_download_only)
    get_cmake_property(vars VARIABLES)
    foreach(var IN LISTS vars)
        if(var MATCHES "^CPM_DECLARATION_.+" AND NOT "${${var}}" STREQUAL "")
            set(decl "${${var}}")
            list(FIND decl DOWNLOAD_ONLY idx)
            if(NOT idx EQUAL -1)
                math(EXPR val_idx "${idx} + 1")
                list(REMOVE_AT decl ${idx} ${val_idx})
                set(${var} "${decl}" PARENT_SCOPE)
            endif()
        endif()
    endforeach()
endfunction()

_dgr_strip_all_download_only()

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


# _dsl_repo_impl(NAME REPO TAG DOWNLOAD_ONLY_MODE [DIR] CPM_ARGS)
#
# The real source (the "home") is always ${DGR_REPOS_DIR}/${NAME}, or the user's
# CPM_<NAME>_SOURCE if set.  It is returned in ${NAME}_SOURCE_DIR.
#
# DIR (or, if not given, DGR_<PROJECT_NAME>_REPOS_DIR}/${NAME} when that variable is set)
# is where the calling project wants the repo to appear.  If that is not the home,
# it becomes a symlink to the home.  Relative paths are relative to the calling
# CMakeLists.txt (CMAKE_CURRENT_SOURCE_DIR).
#
# Two phases:
#  1. FETCH (locked): always a DOWNLOAD_ONLY clone/update of ${home}, plus symlink and
#     .gitignore upkeep.  Nothing here recurses into other dsl_* calls, so the lock
#     can never be re-entered.
#  2. ADD (unlocked, dsl_add_repo only): registered with CPM as ${NAME}_BUILD pointing
#     at the directory CPM actually fetched.  add_subdirectory recurses into the
#     dependency's own dsl_* calls here, with no lock held, so circular dependencies
#     work: CPM registers the name before add_subdirectory, so the cycle stops at its
#     "already added" return.
#
# Get-then-add: CPM treats a DOWNLOAD_ONLY package as already added, so a later add
# under the same name would never call add_subdirectory.  Hence the ${NAME}_BUILD
# registration.  The URL/tag are not used for that call.
# OPTIONS: only dsl_add_repo applies them, and the first add wins.
function(_dsl_repo_impl NAME REPO TAG DOWNLOAD_ONLY_MODE)
    # where real sources live: only a cache value (-DDGR_REPOS_DIR=...) can move it
    if(DEFINED CACHE{DGR_REPOS_DIR})
        get_filename_component(store "$CACHE{DGR_REPOS_DIR}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")
    else()
        set(store "${CMAKE_SOURCE_DIR}/deps")
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

    foreach(arg IN LISTS _dgr_UNPARSED_ARGUMENTS)
        if(arg STREQUAL "SOURCE_DIR" OR arg STREQUAL "DOWNLOAD_ONLY"
                OR arg STREQUAL "NAME" OR arg STREQUAL "GIT_REPOSITORY"
                OR arg STREQUAL "GIT_TAG")
            message(FATAL_ERROR
                    "dsl_get_repo: Direct pass through of CPM ${arg} arg is not supported")
        endif()
    endforeach()

    # ------------------------------------------------------------------
    # Phase 1: FETCH, under the cross-process lock
    # ------------------------------------------------------------------
    get_filename_component(_home_parent "${home}" DIRECTORY)
    get_filename_component(_home_name   "${home}" NAME)
    set(_lockfile "${_home_parent}/.${_home_name}.lock")

    # serialize across processes (parallel CLion profiles).  GUARD FUNCTION is a safety
    # net for the FATAL_ERROR paths; the normal path releases explicitly below.
    file(LOCK "${_lockfile}" GUARD FUNCTION TIMEOUT 300 RESULT_VARIABLE locked)
    if(NOT locked EQUAL 0)
        message(FATAL_ERROR "dsl_get_repo: timed out waiting for ${_lockfile}")
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

    # The declaration replaces this call's args, so put the fetch mode back.
    # The fetch is always DOWNLOAD_ONLY, whether the caller is get or add.
    if(NOT "${CPM_DECLARATION_${NAME}}" STREQUAL "")
        list(APPEND CPM_DECLARATION_${NAME} DOWNLOAD_ONLY YES)
    endif()

    set(first_registration FALSE)
    if(NOT "${NAME}" IN_LIST CPM_PACKAGES)
        set(first_registration TRUE)
    endif()

    CPMAddPackage(
            NAME ${NAME}
            GIT_REPOSITORY ${REPO}
            GIT_TAG ${TAG}
            SOURCE_DIR "${home}"
            DOWNLOAD_ONLY YES
            ${_dgr_UNPARSED_ARGUMENTS})

    if(EXISTS "${home}/.developing")
        message(WARNING "The override is set by the existence of ${home}/.developing\n Delete it to allow auto update." )
    endif()

    # Only claim the name if we registered it.  If someone else (a plain CPMAddPackage)
    # got there first, the fetch above was a no-op and the name is not ours to re-add.
    if(first_registration)
        set_property(GLOBAL PROPERTY DGR_FETCHED_${NAME} TRUE)
    endif()

    if(NOT overridden)
        git_ignore_entry("${home}")
    endif()

    if(NOT wanted STREQUAL home)
        make_symlink("${wanted}" "${home}")
        git_ignore_entry("${wanted}")
    endif()

    # Release now, before anything can recurse.
    file(LOCK "${_lockfile}" RELEASE)

    # ------------------------------------------------------------------
    # Phase 2: ADD, no lock held
    # ------------------------------------------------------------------
    set(pkg "${NAME}")
    get_property(fetched GLOBAL PROPERTY DGR_FETCHED_${NAME})
    if(NOT DOWNLOAD_ONLY_MODE AND fetched)
        # Add what CPM actually fetched (env override, lock declaration, local package)
        set(src "${${NAME}_SOURCE_DIR}")
        if(src STREQUAL "" OR NOT IS_DIRECTORY "${src}")
            message(WARNING "dsl_add_repo: nothing to add for ${NAME} "
                    "(no source dir; found via find_package?)")
        else()
            set(pkg "${NAME}_BUILD")
            set(CPM_PACKAGE_LOCK_ENABLED FALSE)

            # Pin the binary dir to the one phase 1 reported, so it is known before the
            # add starts (a cycle re-entering mid-add can't see CPM's own record yet).
            set(bin_args "")
            if(NOT "${${NAME}_BINARY_DIR}" STREQUAL "")
                set(bin_args BINARY_DIR "${${NAME}_BINARY_DIR}")
            endif()

            CPMAddPackage(
                    NAME ${pkg}
                    SOURCE_DIR "${src}"
                    ${bin_args}
                    DOWNLOAD_ONLY NO
                    FORCE YES        # skip CPM's find_package(${pkg}) lookup
                    ${_dgr_UNPARSED_ARGUMENTS})
            if(${pkg}_ADDED)
                cpm_create_module_file(${NAME})
            endif()
        endif()
    endif()

    # Keep phase 1's values unless phase 2 has real ones (empty inside a cycle).
    if(NOT pkg STREQUAL NAME)
        foreach(var SOURCE_DIR BINARY_DIR)
            if(NOT "${${pkg}_${var}}" STREQUAL "")
                set(${NAME}_${var} "${${pkg}_${var}}")
            endif()
        endforeach()
        set(${NAME}_ADDED "${${pkg}_ADDED}")
    endif()

    # pass results to the caller under the plain name
    foreach(var SOURCE_DIR BINARY_DIR ADDED)
        set(${NAME}_${var} "${${NAME}_${var}}" PARENT_SCOPE)
    endforeach()
    set(CPM_LAST_PACKAGE_NAME "${NAME}" PARENT_SCOPE)
endfunction()

# DOWNLOAD_ONLY mode: fetch only, no add_subdirectory, any OPTIONS are not applied
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