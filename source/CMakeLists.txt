cmake_minimum_required(VERSION 3.1)

if (WIN32)
   add_definitions(-DCUBEWORLD_PLATFORM_WINDOWS)
   add_definitions("-W4 /wd4201 /wd4068 /wd26451 /wd26444 /wd26439 /wd26498")
else()
   add_definitions(-DCUBEWORLD_PLATFORM_MACOSX)
endif()
add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
add_definitions(-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)
add_definitions(-DNOMINMAX)

# Allow for referencing, e.g. <RGBDesignPatterns/Scope.h>
# from any of these projects.
include_directories(.)
include_directories(Library)

# Order is important
add_subdirectory(Engine)
add_subdirectory(Library)
add_subdirectory(Shared)
add_subdirectory(DataCLI)
add_subdirectory(Editor)
add_subdirectory(Game)
add_subdirectory(Tests)

# Nobody cares what you do here
add_subdirectory(Sandbox EXCLUDE_FROM_DEFAULT_BUILD)