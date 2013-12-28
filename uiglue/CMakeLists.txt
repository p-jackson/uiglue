file(GLOB_RECURSE INCS "*.h")
file(GLOB_RECURSE SRCS "*.cc")

add_library(uiglue ${SRCS} ${INCS})

# Create precompiled header with cotire (we don't need a unity build)
if(NOT CMAKE_COMPILER_IS_GNUCXX)
  set_target_properties(uiglue PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
  cotire(uiglue)
endif()
