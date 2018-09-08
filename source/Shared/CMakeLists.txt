cmake_minimum_required(VERSION 3.1)
project(Shared)

if (APPLE)
   file(GLOB PLATFORM_SOURCES "Platform/OSX/*")
else()
   file(GLOB PLATFORM_SOURCES "Platform/Windows/*")
endif()

file(GLOB SOURCES
   "*.h"
   "*.cpp"
   "**/*.h"
   "**/*.cpp"
   "Shaders/*"
   ${PLATFORM_SOURCES}
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} )
target_link_libraries(${PROJECT_NAME} ${LIBS} Engine)

# Nice-looking separation for VS2017
source_group(Components REGULAR_EXPRESSION ".*/Components/*")
source_group(Event REGULAR_EXPRESSION ".*/Event/*")
source_group(Helpers REGULAR_EXPRESSION ".*/Helpers/*")
source_group(Renderers REGULAR_EXPRESSION ".*/Renderers/*")
source_group(Shaders REGULAR_EXPRESSION ".*/Shaders/*")
source_group(States REGULAR_EXPRESSION ".*/States/*")
source_group(Systems REGULAR_EXPRESSION ".*/Systems/*")

set(SHARED_SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders" PARENT_SCOPE)
