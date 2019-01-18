cmake_minimum_required(VERSION 3.1)
project(DataCLI)

file(GLOB SOURCES
   "*.h"
   "*.cpp"
   "**/*.h"
   "**/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} ${CW_LIBS} Engine Shared)

if (WIN32)
   set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
endif()

# install libraries into /lib
INSTALL( TARGETS DataCLI DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
