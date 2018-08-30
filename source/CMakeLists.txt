cmake_minimum_required(VERSION 3.1)

if (WIN32)
   add_definitions(-DCUBEWORLD_PLATFORM_WINDOWS)
else()
   add_definitions(-DCUBEWORLD_PLATFORM_MACOSX)
endif()

ADD_SUBDIRECTORY(DataCLI)
ADD_SUBDIRECTORY(Engine)
ADD_SUBDIRECTORY(Game)
ADD_SUBDIRECTORY(Sandbox)
