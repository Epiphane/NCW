# Add GLFW
set(VERSION "1.0.0")

if (WIN32)
add_definitions("/W0")
endif()
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}")
list(APPEND CW_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}/src)
list(APPEND CW_LIBS noise-static)
set_target_properties(noise PROPERTIES FOLDER "_")

# Propagate variables back up
set(CW_INCLUDES ${CW_INCLUDES} PARENT_SCOPE)
set(CW_LIBS ${CW_LIBS} PARENT_SCOPE)
