# Define the files in this project
file(GLOB_RECURSE includes "*.h")
file(GLOB_RECURSE sources "*.cc")
set(all_files ${sources} ${includes})

# Create the executable test project
add_executable(tests ${all_files})

# googletest setup
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

target_link_libraries(tests uiglue curt comctl32 ${GTEST_BOTH_LIBRARIES})

add_test(NAME uiglue-unit-tests COMMAND tests)
