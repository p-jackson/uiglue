cmake_minimum_required(VERSION 2.8.8)
project(catch_builder CXX)
include(ExternalProject)

# FindGit makes the GIT_EXECUTABLE and GIT_FOUND variables available

if(NOT GIT_FOUND)
  message(FATAL_ERROR "FindGit must be run before including the catch project "
          "e.g. find_package(Git)")
endif()

set(catch_patch ${CMAKE_BINARY_DIR}/catch.patch)
configure_file(catch.patch ${catch_patch} NEWLINE_STYLE UNIX)

ExternalProject_Add(
  catch
  GIT_REPOSITORY https://github.com/philsquared/Catch.git
  GIT_TAG e45e3a139ad46de2a38549d877e2ec0b2be5bdab
  TIMEOUT 10
  PATCH_COMMAND ${GIT_EXECUTABLE} apply ${catch_patch}
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
)

# Expose required variable (CATCH_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(catch source_dir)
set(
  CATCH_INCLUDE_DIR
  ${source_dir}/include
  CACHE INTERNAL "Path to include folder for Catch"
)
