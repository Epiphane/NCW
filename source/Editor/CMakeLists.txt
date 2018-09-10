cmake_minimum_required(VERSION 3.1)
project(Editor)

file(GLOB SOURCES
   "*.h"
   "*.cpp"
   "**/*.h"
   "**/*.cpp"
   "Shaders/*"
)

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} ${CW_LIBS} Engine Shared)

if (WIN32)
   set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

# Nice-looking separation for VS2017
foreach(source IN LISTS SOURCES)
   file(RELATIVE_PATH source_relative "${CMAKE_CURRENT_SOURCE_DIR}" "${source}")
   get_filename_component(source_path "${source_relative}" PATH)
   string(REPLACE "/" "\\" source_path_msvc "${source_path}")
   source_group("${source_path_msvc}" FILES "${source}")
endforeach()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_SOURCE_DIR}/Assets ${PROJECT_BINARY_DIR}/Assets)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${SHARED_SHADERS_DIR} ${PROJECT_BINARY_DIR}/Shaders)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_SOURCE_DIR}/Shaders ${PROJECT_BINARY_DIR}/Shaders)
