project(uiglue)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(EXT_PROJECTS_DIR "${CMAKE_SOURCE_DIR}/third_party")

enable_testing()

include(cotire)

# Boost setup
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.55 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# All projects can include headers relative to the root of the repository
include_directories(${uiglue_SOURCE_DIR})

if(MSVC)
  add_definitions(/W4)
endif()

add_subdirectory(${EXT_PROJECTS_DIR}/catch)

add_subdirectory(curt)
add_subdirectory(bind_test)
add_subdirectory(hello_world)
add_subdirectory(tests)
add_subdirectory(uiglue)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET catch PROPERTY FOLDER "third_party")
