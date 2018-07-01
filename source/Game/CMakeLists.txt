cmake_minimum_required(VERSION 3.1)

SET(PROJECT_NAME Game)

include_directories(${INCLUDES})
include_directories(..)

file(GLOB SOURCES
   "*.h"
   "*.cpp"
   "**/*.h"
   "**/*.cpp"
   "Shaders/*"
)

# add_library(${PROJECT_NAME} SHARED ${SOURCES} )
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/source/Game")
target_link_libraries(${PROJECT_NAME} ${LIBS} glew Engine)

if (WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	ADD_DEFINITIONS( "-W4 /wd4201" )
endif()

# install libraries into /lib
INSTALL( TARGETS Game DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
SOURCE_GROUP(Components REGULAR_EXPRESSION ".*/Components/*")
SOURCE_GROUP(Renderers REGULAR_EXPRESSION ".*/Renderers/*")
SOURCE_GROUP(Shaders REGULAR_EXPRESSION ".*/Shaders/*")
SOURCE_GROUP(States REGULAR_EXPRESSION ".*/States/*")
SOURCE_GROUP(Systems REGULAR_EXPRESSION ".*/Systems/*")

file(COPY "${CMAKE_SOURCE_DIR}/source/Game/Shaders" DESTINATION ${PROJECT_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/source/Game/Fonts" DESTINATION ${PROJECT_BINARY_DIR})
