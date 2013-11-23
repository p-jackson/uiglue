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
target_link_libraries(uiglue ${Boost_LIBRARIES})

# Run the mt tool as a post build to embed uiglue.exe.manifest
get_property(uiglue_exe_location TARGET uiglue PROPERTY LOCATION)
add_custom_command(
  TARGET uiglue
  POST_BUILD
  COMMAND "mt.exe" -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\uiglue.exe.manifest\" -outputresource:\"${uiglue_exe_location}\"\;\#1 /nologo
  COMMENT "Adding manifest"
)


# Build rules for copying view files into the binary directory
message(STATUS "Found view files:")

# Function adds a rule for copying a file to the binary directory
function(copy_view_file FILE DEST)
  get_filename_component(FILENAME "${FILE}" NAME)
  set(input ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
  set(output ${CMAKE_CURRENT_BINARY_DIR}/${DEST}${FILENAME})

  message(STATUS "  ${input}")

  add_custom_command(
    OUTPUT "${output}"
    MAIN_DEPENDENCY "${input}"
    COMMAND ${CMAKE_COMMAND} -E copy "${input}" "${output}"
  )
endfunction()

# Create folder for the views
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../views")

# Copy view files to binary directory during build
file(GLOB view_files "../views/*")
foreach(view ${view_files})
  file(RELATIVE_PATH relative_path ${CMAKE_CURRENT_SOURCE_DIR} ${view})
  copy_view_file(${relative_path} "../views/")
endforeach()

# Copy resource header to binary directory during build
copy_view_file("resource.h" "")
