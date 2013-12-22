# Define the files in this project
file(GLOB_RECURSE includes "*.h")
file(GLOB_RECURSE sources "*.cc")
set(all_files ${sources} ${includes})

# LPTSTR should be defined as wchar_t*
add_definitions(-DUNICODE)

# Create the executable hello_world project
add_executable(hello_world WIN32 ${all_files})

# Link to static libraries
target_link_libraries(hello_world curt uiglue comctl32)

# Create precompiled header with cotire (we don't need a unity build)
set_target_properties(hello_world PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(hello_world)
