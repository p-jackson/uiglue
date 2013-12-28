file(GLOB_RECURSE INCS "*.h")
file(GLOB_RECURSE SRCS "*.cc")

add_library(curt ${SRCS} ${INCS})

# Create precompiled header with cotire (we don't need a unity build)
set_target_properties(curt PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(curt)
