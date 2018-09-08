# Add glad
set(VERSION "09072018")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}/include)
add_library(glad STATIC "${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}/src/glad.c")

list(APPEND CW_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}/include)
list(APPEND CW_LIBS glad)

# Propagate variables back up
set(CW_INCLUDES ${CW_INCLUDES} PARENT_SCOPE)
set(CW_LIBS ${CW_LIBS} PARENT_SCOPE)