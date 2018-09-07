cmake_minimum_required(VERSION 3.1)
project(Engine)

file(GLOB SOURCES
   "*.h"
   "*.cpp"
   "**/*.h"
   "**/*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} )
target_link_libraries(${PROJECT_NAME} ${CW_LIBS})

# Nice-looking separation for VS2017
source_group(Logger REGULAR_EXPRESSION ".*/Engine/Logger/.*$")
source_group(Graphics REGULAR_EXPRESSION ".*/Engine/Graphics/.*$")
source_group(Core REGULAR_EXPRESSION ".*/Engine/Core/.*$")
source_group(GameObject REGULAR_EXPRESSION ".*/Engine/GameObject/.*$")
source_group(Entity REGULAR_EXPRESSION ".*/Engine/Entity/.*$")
source_group(Event REGULAR_EXPRESSION ".*/Engine/Event/.*$")
source_group(System REGULAR_EXPRESSION ".*/Engine/System/.*$")
