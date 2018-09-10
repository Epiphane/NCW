cmake_minimum_required(VERSION 3.1)
project(Shared)

file(GLOB SOURCES
   "*.h"
   "*.cpp"
   "**/*.h"
   "**/*.cpp"
   "Shaders/*"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} )
target_link_libraries(${PROJECT_NAME} ${LIBS} Engine)

# Nice-looking separation for VS2017
foreach(source IN LISTS SOURCES)
   file(RELATIVE_PATH source_relative "${CMAKE_CURRENT_SOURCE_DIR}" "${source}")
   get_filename_component(source_path "${source_relative}" PATH)
   string(REPLACE "/" "\\" source_path_msvc "${source_path}")
   source_group("${source_path_msvc}" FILES "${source}")
endforeach()

set(SHARED_SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders" PARENT_SCOPE)
