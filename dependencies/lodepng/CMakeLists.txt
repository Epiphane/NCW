# Add GLFW
set(VERSION "20131222")

if (WIN32)
add_definitions("/W0")
endif()
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}")
list(APPEND CW_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}")
list(APPEND CW_LIBS lodepng)

# Propagate variables back up
set(CW_INCLUDES ${CW_INCLUDES} PARENT_SCOPE)
set(CW_LIBS ${CW_LIBS} PARENT_SCOPE)
