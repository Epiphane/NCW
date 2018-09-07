# Add glew
set(VERSION "1.11.0")
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}/include)
add_library(glew STATIC ${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}/src/glew.c )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGLEW_STATIC")
set_target_properties(glew PROPERTIES OUTPUT_NAME glew PREFIX lib)

list(APPEND CW_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}/include)
list(APPEND CW_LIBS glew)

# Propagate variables back up
set(CW_INCLUDES ${CW_INCLUDES} PARENT_SCOPE)
set(CW_LIBS ${CW_LIBS} PARENT_SCOPE)