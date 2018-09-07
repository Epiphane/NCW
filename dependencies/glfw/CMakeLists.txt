set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")

# Add GLFW
set(VERSION "3.1.0")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}")
list(APPEND CW_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}/include)
list(APPEND CW_LIBS glfw ${GLFW_LIBRARIES})

# Propagate variables back up
set(CW_INCLUDES ${CW_INCLUDES} PARENT_SCOPE)
set(CW_LIBS ${CW_LIBS} PARENT_SCOPE)
