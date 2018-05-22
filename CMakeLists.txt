cmake_minimum_required(VERSION 3.1)
project(CubeWorld)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("/std:c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
   message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 -DGLEW_STATIC")

set(SRC_DIR "${PROJECT_SOURCE_DIR}/source")
set(DEP_DIR "${PROJECT_SOURCE_DIR}/dependencies")
set(BUILD_DIR "${PROJECT_SOURCE_DIR}/build")

# GLFW
set(GLFW_DIR "${DEP_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(${GLFW_DIR})
list(APPEND INCLUDES
   ${GLFW_DIR}/include
)
list(APPEND LIBS
   glfw ${GLFW_LIBRARIES}
)

# glew
set(GLEW_DIR "${DEP_DIR}/glew")
list(APPEND INCLUDES
   ${GLEW_DIR}/include
)

add_library(glew STATIC ${GLEW_DIR}/src/glew.c )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGLEW_STATIC")
set_target_properties(glew PROPERTIES OUTPUT_NAME glew PREFIX lib)
list(APPEND LIBS
    glew
)

#glm
list(APPEND INCLUDES
   ${DEP_DIR}
)

#curl
#add_subdirectory("${DEP_DIR}/curl")
#list(APPEND INCLUDES
#   "${DEP_DIR}/curl/include"
#   "${BUILD_DIR}/${DEP_DIR}/curl/include"
#)
#list(APPEND LIBS
#   libcurl
#)

#noise
set(NOISE_DIR "${DEP_DIR}/noise")
add_subdirectory(${NOISE_DIR})
list(APPEND INCLUDES
   ${NOISE_DIR}/include
)
list(APPEND LIBS
   libnoise
)

# FMOD
set(FMOD_DIR "${DEP_DIR}/FMOD")
set(FMOD_INCLUDE_DIR "${FMOD_DIR}/include")
list(APPEND INCLUDES
   ${FMOD_INCLUDE_DIR}
)

if (APPLE)
   set(FMOD_LIBRARY_RELEASE "${FMOD_DIR}/lib/osx/release/libfmod.dylib")
   set(FMOD_LIBRARY_DEBUG "${FMOD_DIR}/lib/osx/debug/libfmodL.dylib")
   list(APPEND LIBS
   ${FMOD_LIBRARY_RELEASE}
      ${FMOD_LIBRARY_DEBUG}
   )
endif (APPLE)

if (WIN32)
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMOD_WIN_ARCHITECTURE "x86_64")
	  set(FMOD_WIN_LIB "fmod64_vc.lib")
	  set(FMOD_WIN_LIB_DEBUG "fmodL64_vc.lib")
	  set(FMOD_DLL "fmod64.dll")
   else (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMOD_WIN_ARCHITECTURE "x86")
	  set(FMOD_WIN_LIB "fmod_vc.lib")
	  set(FMOD_WIN_LIB_DEBUG "fmodL_vc.lib")
	  set(FMOD_DLL "fmod.dll")
   endif (CMAKE_SIZEOF_VOID_P EQUAL 8)

   set(FMOD_LIBRARY_RELEASE "${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/release/${FMOD_WIN_LIB}")
   set(FMOD_LIBRARY_DEBUG "${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/debug/${FMOD_WIN_LIB_DEBUG}")
   list(APPEND LIBS
      ${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/release/${FMOD_WIN_LIB}
      ${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/debug/${FMOD_WIN_LIB_DEBUG}
   )
      file(COPY ${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/release/${FMOD_DLL} DESTINATION ${PROJECT_BINARY_DIR})
endif (WIN32)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMOD_LINUX_ARCHITECTURE "x86_64")
   else (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMOD_LINUX_ARCHITECTURE "x86")
   endif (CMAKE_SIZEOF_VOID_P EQUAL 8)

   set(FMOD_LIBRARY_RELEASE "${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so.5.10")
   set(FMOD_LIBRARY_DEBUG "${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so.5.10")
   list(APPEND LIBS
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so.5.10
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so.5
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so.5.10
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so.5
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so
   )
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# dl libs
list(APPEND LIBS
   ${CMAKE_DL_LIBS}
)

if (WIN32)
   file(GLOB RESOURCES
      "${SRC_DIR}/shaders/*"
      "${SRC_DIR}/models/*"
      "${SRC_DIR}/textures/*"
      "${SRC_DIR}/audio/*"
      "${SRC_DIR}/beatmaps/*"
      "${SRC_DIR}/songs.txt"
   )

   file(COPY ${ABS_RESOURCES} DESTINATION ${PROJECT_BINARY_DIR})

   # Find user and system name
	#SET(SYSTEM_NAME $ENV{USERDOMAIN} CACHE STRING SystemName)
	#SET(USER_NAME $ENV{USERNAME} CACHE STRING UserName)

	# Configure the template file
	#SET(USER_FILE ${_projectName}.vcproj.${SYSTEM_NAME}.${USER_NAME}.user)
	#SET(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${USER_FILE})
	#CONFIGURE_FILE(UserTemplate.user ${USER_FILE} @ONLY)
endif (WIN32)

add_subdirectory(${SRC_DIR})

file(GLOB SOURCES
   "${SRC_DIR}/*.h"
   "${SRC_DIR}/*.cpp"
   "${SRC_DIR}/**/*.h"
   "${SRC_DIR}/**/*.cpp"
)

file(GLOB SHADERS
    "${SRC_DIR}/shaders/*.glsl"
)

include_directories(${INCLUDES})
# add_executable(${PROJECT_NAME} ${SOURCES} ${LIB_SOURCES} ${SHADERS})
# target_link_libraries(${PROJECT_NAME} ${LIBS})

# set_target_properties(CubeWorld PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source/")
# create_target_launcher(CubeWorld WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/source/")
# create_default_target_launcher(CubeWorld WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/source/")

# set_target_properties(noise noise-static PROPERTIES FOLDER "noise")

SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*glsl$" )
SOURCE_GROUP(Lib REGULAR_EXPRESSION ".*/Lib/.*$" )
