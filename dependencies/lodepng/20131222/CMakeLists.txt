SET(PROJECT_NAME lodepng)

set(libSrcs 
   lodepng/lodepng.cpp
)

add_library( lodepng STATIC ${libSrcs} )

# i would like to see more projects using these defaults
if (!WIN32)
ADD_DEFINITIONS( "-Wall -ansi -pedantic -O3" )
endif()
