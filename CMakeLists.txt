project(uiglue)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(EXT_PROJECTS_DIR "${CMAKE_SOURCE_DIR}/deps")

enable_testing()

include(cotire)

# Boost setup
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)

# The SYSTEM flag silences warnings from Boost on GCC
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

# All projects can include headers relative to the root of the repository
include_directories(${uiglue_SOURCE_DIR})

# LPTSTR should be defined as wchar_t*
add_definitions(-DUNICODE)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
endif()

add_subdirectory(${EXT_PROJECTS_DIR}/catch)

add_subdirectory(curt)
add_subdirectory(tests)
add_subdirectory(uiglue)
add_subdirectory(examples/bind_test)
add_subdirectory(examples/hello_world)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET catch PROPERTY FOLDER "deps")
set_property(TARGET bind_test PROPERTY FOLDER "examples")
set_property(TARGET hello_world PROPERTY FOLDER "examples")
