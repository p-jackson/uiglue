# Define the files in this project
file(GLOB_RECURSE includes "*.h")
file(GLOB_RECURSE sources "*.cc")
file(GLOB_RECURSE views "../views/*.json")
set(resources "uiglue.rc" "uiglue.exe.manifest")
set(all_files ${sources} ${includes} ${views} ${resources})

# LPTSTR should be defined as wchar_t*
add_definitions(-DUNICODE)

# Create the executable uiglue project
add_executable(uiglue WIN32 ${all_files})

# Create filters within Visual Studio for resources and views
source_group(Views FILES ${views})
source_group("Resource Files" FILES ${resources})

# Link to static libraries
target_link_libraries(uiglue curt comctl32)

# Create precompiled header with cotire (we don't need a unity build)
set_target_properties(uiglue PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(uiglue)

# Run the mt tool as a post build to embed uiglue.exe.manifest
get_property(uiglue_exe_location TARGET uiglue PROPERTY LOCATION)
add_custom_command(
  TARGET uiglue
  POST_BUILD
  COMMAND "mt.exe" -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\uiglue.exe.manifest\" -outputresource:\"${uiglue_exe_location}\"\;\#1 /nologo
  COMMENT "Adding manifest"
)
