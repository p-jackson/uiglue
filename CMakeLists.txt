project(uiglue)

cmake_minimum_required(VERSION 2.8)

# Boost setup
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.55 COMPONENTS filesystem system REQUIRED)
add_definitions(-DBOOST_ALL_NO_LIB)
include_directories(${Boost_INCLUDE_DIR})

# All projects can include headers relative to the root of the repository
include_directories(${uiglue_SOURCE_DIR})

add_subdirectory(uiglue)
