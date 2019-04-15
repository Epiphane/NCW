cmake_minimum_required(VERSION 3.1)

# Order is not important
add_subdirectory(RGBBinding)
add_subdirectory(RGBDesignPatterns)
add_subdirectory(RGBFileSystem)
add_subdirectory(RGBLogger)
add_subdirectory(RGBText)

set(CW_LIBS ${CW_LIBS} PARENT_SCOPE)
