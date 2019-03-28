cmake_minimum_required(VERSION 3.1)

if (WIN32)
   add_definitions(-DCUBEWORLD_PLATFORM_WINDOWS)
   add_definitions("-W4 /wd4201 /wd4068")
else()
   add_definitions(-DCUBEWORLD_PLATFORM_MACOSX)
endif()

# Allow for referencing, e.g. <Engine/Core/Scope.h>
# from any of these projects.
include_directories(.)

# Order is important
add_subdirectory(Engine)
add_subdirectory(Shared)
add_subdirectory(DataCLI)
add_subdirectory(Editor)
add_subdirectory(Game)

# Nobody cares what you do here
add_subdirectory(Sandbox EXCLUDE_FROM_ALL)