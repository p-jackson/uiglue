project(uiglue)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(cotire)

# Boost setup
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.55)
include_directories(${Boost_INCLUDE_DIR})

# All projects can include headers relative to the root of the repository
include_directories(${uiglue_SOURCE_DIR})

string(TOLOWER ${CMAKE_BUILD_TOOL} build_tool)
if(build_tool MATCHES "(msdev|devenv|nmake|msbuild)")
  add_definitions(/W4)
endif()

add_subdirectory(curt)
add_subdirectory(hello_world)
add_subdirectory(test)
add_subdirectory(uiglue)
