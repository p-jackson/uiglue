# Define the files in this project
file(GLOB_RECURSE includes "*.h")
file(GLOB_RECURSE sources "*.cc")
set(resources "dialog.rc" "dialog.exe.manifest")
set(all_files ${sources} ${includes} ${resources})

add_executable(dialog WIN32 ${all_files})

# Create filters within Visual Studio for resources
source_group("Resource Files" FILES ${resources})

# Link to static libraries
target_link_libraries(dialog curt uiglue comctl32)

# Create precompiled header with cotire (we don't need a unity build)
set_target_properties(dialog PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(dialog)

if(MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -municode")
endif()

if(MSVC)
  # Run the mt tool as a post build to embed dialog.exe.manifest
  get_property(dialog_exe_location TARGET dialog PROPERTY LOCATION)
  add_custom_command(
    TARGET dialog
    POST_BUILD
    COMMAND "mt.exe" -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\dialog.exe.manifest\" -outputresource:\"${dialog_exe_location}\"\;\#1 /nologo
    COMMENT "Adding manifest"
  )
endif()
