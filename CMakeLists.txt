cmake_minimum_required(VERSION 3.1)
project(CubeWorld)

if (APPLE)
    set(CMAKE_OSX_SYSROOT macosx)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14")
endif()

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# @rpath stuff on mac. I don't really understand it yet
set(CMAKE_MACOSX_RPATH 1)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/source")
set(DEP_DIR "${PROJECT_SOURCE_DIR}/dependencies")

add_subdirectory("${DEP_DIR}/glfw")
add_subdirectory("${DEP_DIR}/glad")
add_subdirectory("${DEP_DIR}/glm")
if (WIN32)
else()
    # TODO either make libfswatch work on windows or do it ourselves
    add_subdirectory("${DEP_DIR}/libfswatch")
endif()
add_subdirectory("${DEP_DIR}/libnoise")
add_subdirectory("${DEP_DIR}/libyaml")
add_subdirectory("${DEP_DIR}/lodepng")
add_subdirectory("${DEP_DIR}/noiseutils")
add_subdirectory("${DEP_DIR}/freetype")
add_subdirectory("${DEP_DIR}/rapidjson")
add_subdirectory("${DEP_DIR}/rhea")
add_subdirectory("${DEP_DIR}/sqlite")
add_subdirectory("${DEP_DIR}/FMOD")
add_subdirectory("${DEP_DIR}/zlib")

foreach(lib IN LISTS CW_LIBS)
set_target_properties(${lib} PROPERTIES FOLDER "dependencies")
endforeach()

# Ensure everything after draws from header files
# specified in the dependencies.
include_directories(${CW_INCLUDES})

# NCW source
# Include it in ./build, instead of ./build/source
add_subdirectory(${SRC_DIR} "source")
