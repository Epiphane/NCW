cmake_minimum_required(VERSION 3.1)

SET(PROJECT_NAME Shared)

include_directories(${INCLUDES})
include_directories(..)

file(GLOB SOURCES
   "*.h"
   "*.cpp"
   "**/*.h"
   "**/*.cpp"
   "Shaders/*"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} )
target_link_libraries(${PROJECT_NAME} ${LIBS} glew Engine)

# install libraries into /lib
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )

SOURCE_GROUP(Components REGULAR_EXPRESSION ".*/Components/*")
SOURCE_GROUP(Event REGULAR_EXPRESSION ".*/Event/*")
SOURCE_GROUP(Helpers REGULAR_EXPRESSION ".*/Helpers/*")
SOURCE_GROUP(Renderers REGULAR_EXPRESSION ".*/Renderers/*")
SOURCE_GROUP(Shaders REGULAR_EXPRESSION ".*/Shaders/*")
SOURCE_GROUP(States REGULAR_EXPRESSION ".*/States/*")
SOURCE_GROUP(Systems REGULAR_EXPRESSION ".*/Systems/*")

file(COPY "${CMAKE_SOURCE_DIR}/source/Shared/Shaders" DESTINATION ${PROJECT_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/Assets" DESTINATION ${PROJECT_BINARY_DIR})
