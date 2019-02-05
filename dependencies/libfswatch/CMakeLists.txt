# Add libfswatch
set(VERSION "0.2.4")

if (WIN32)
add_definitions("/W0")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}")
list(APPEND CW_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}/src")
list(APPEND EDITOR_LIBS libfswatch)

# Propagate variables back up
set(CW_INCLUDES ${CW_INCLUDES} PARENT_SCOPE)
set(EDITOR_LIBS ${EDITOR_LIBS} PARENT_SCOPE)
