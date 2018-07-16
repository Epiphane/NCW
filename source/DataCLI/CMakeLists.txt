cmake_minimum_required(VERSION 3.1)

SET(PROJECT_NAME DataCLI)

include_directories(${INCLUDES})
include_directories(..)

file(GLOB SOURCES
   "*.h"
   "*.cpp"
   "**/*.h"
   "**/*.cpp"
)

# add_library(${PROJECT_NAME} SHARED ${SOURCES} )
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/source/DataCLI")
target_link_libraries(${PROJECT_NAME} ${LIBS} Engine)

if (WIN32)
   set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
   ADD_DEFINITIONS( "-W4 /wd4201" )
else()
endif()

# install libraries into /lib
INSTALL( TARGETS DataCLI DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
