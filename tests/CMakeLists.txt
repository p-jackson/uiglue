# Define the files in this project
file(GLOB_RECURSE includes "*.h")
file(GLOB_RECURSE sources "*.cc")
set(all_files ${sources} ${includes})

# Create the executable test project
add_executable(tests ${all_files})

include_directories(${CATCH_INCLUDE_DIR})

target_link_libraries(tests uiglue curt comctl32)

add_dependencies(tests catch)

add_test(NAME uiglue-unit-tests COMMAND tests)
