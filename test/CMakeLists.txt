# Define the files in this project
file(GLOB_RECURSE includes "*.h")
file(GLOB_RECURSE sources "*.cc")
set(resources "uiglue.rc" "uiglue.exe.manifest")
set(all_files ${sources} ${includes} ${resources})

# LPTSTR should be defined as wchar_t*
add_definitions(-DUNICODE)

# Create the executable test project
add_executable(test WIN32 ${all_files})

# Create filters within Visual Studio for resources
source_group("Resource Files" FILES ${resources})

# Link to static libraries
target_link_libraries(test curt uiglue comctl32)

# Create precompiled header with cotire (we don't need a unity build)
set_target_properties(test PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(test)

# Run the mt tool as a post build to embed uiglue.exe.manifest
get_property(test_exe_location TARGET test PROPERTY LOCATION)
add_custom_command(
  TARGET test
  POST_BUILD
  COMMAND "mt.exe" -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\uiglue.exe.manifest\" -outputresource:\"${test_exe_location}\"\;\#1 /nologo
  COMMENT "Adding manifest"
)
