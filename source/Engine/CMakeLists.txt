cmake_minimum_required(VERSION 3.1)
project(Engine)

file(GLOB_RECURSE OSX_SOURCES "*/OSX/*")
file(GLOB_RECURSE WIN_SOURCES "*/Windows/*")

file(GLOB_RECURSE SOURCES
   "*.h"
   "*.cpp"
   "*.mm"
)

if (APPLE)
list(REMOVE_ITEM SOURCES ${WIN_SOURCES})
elseif(WIN32)
list(REMOVE_ITEM SOURCES ${OSX_SOURCES})
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES} )
target_link_libraries(${PROJECT_NAME} ${CW_LIBS})

# Nice-looking separation for VS2017
source_group(Loggers REGULAR_EXPRESSION ".*/Engine/Logger/.*$")
source_group(Graphics REGULAR_EXPRESSION ".*/Engine/Graphics/.*$")
source_group(Core REGULAR_EXPRESSION ".*/Engine/Core/.*$")
source_group(GameObject REGULAR_EXPRESSION ".*/Engine/GameObject/.*$")
source_group(Entity REGULAR_EXPRESSION ".*/Engine/Entity/.*$")
source_group(Event REGULAR_EXPRESSION ".*/Engine/Event/.*$")
source_group(System REGULAR_EXPRESSION ".*/Engine/System/.*$")

foreach(source IN LISTS SOURCES)
   file(RELATIVE_PATH source_relative "${CMAKE_CURRENT_SOURCE_DIR}" "${source}")
   get_filename_component(source_path "${source_relative}" PATH)
   string(REPLACE "/" "\\" source_path_msvc "${source_path}")
   source_group("${source_path_msvc}" FILES "${source}")
endforeach()
