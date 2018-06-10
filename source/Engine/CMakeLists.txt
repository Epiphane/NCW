cmake_minimum_required(VERSION 3.1)

SET(PROJECT_NAME Engine)

include_directories(${INCLUDES})
INCLUDE_DIRECTORIES (noise)
INCLUDE_DIRECTORIES (..)

file(GLOB SOURCES
   "*.h"
   "*.cpp"
   "**/*.h"
   "**/*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} )
target_link_libraries(${PROJECT_NAME} ${LIBS} glew)

ADD_DEFINITIONS( "-W4 /wd4201" )

# install libraries into /lib
INSTALL(TARGETS Engine DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
SOURCE_GROUP(Logger REGULAR_EXPRESSION ".*/Engine/Logger/.*$")
SOURCE_GROUP(Graphics REGULAR_EXPRESSION ".*/Engine/Graphics/.*$")
SOURCE_GROUP(Core REGULAR_EXPRESSION ".*/Engine/Core/.*$")
SOURCE_GROUP(GameObject REGULAR_EXPRESSION ".*/Engine/GameObject/.*$")
SOURCE_GROUP(Entity REGULAR_EXPRESSION ".*/Engine/Entity/.*$")
SOURCE_GROUP(System REGULAR_EXPRESSION ".*/Engine/System/.*$")