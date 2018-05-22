cmake_minimum_required(VERSION 3.1)

SET(PROJECT_NAME libnoise)

include_directories(${INCLUDES})
INCLUDE_DIRECTORIES (include)

file(GLOB SOURCES
   "**/*.h"
   "**/*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} )
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "noise")

ADD_DEFINITIONS( "-W4 -DGLEW_STATIC" )

# install libraries into /lib
INSTALL( TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )