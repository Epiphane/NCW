cmake_minimum_required(VERSION 3.1)
project(RGBText)

file(GLOB_RECURSE SOURCES
   "*.h"
   "*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Library")
target_link_libraries(${PROJECT_NAME} ${CW_LIBS})

# Nice-looking separation for VS2017
foreach(source IN LISTS SOURCES)
   file(RELATIVE_PATH source_relative "${CMAKE_CURRENT_SOURCE_DIR}" "${source}")
   get_filename_component(source_path "${source_relative}" PATH)
   string(REPLACE "/" "\\" source_path_msvc "${source_path}")
   source_group("${source_path_msvc}" FILES "${source}")
endforeach()

list(APPEND CW_LIBS ${PROJECT_NAME})
set(CW_LIBS ${CW_LIBS} PARENT_SCOPE)
