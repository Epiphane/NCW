# http://www.linux-magazin.de/Heft-Abo/Ausgaben/2007/02/Mal-ausspannen

SET(PROJECT_NAME noiseutils)

set(libSrcs 
   noiseutils/noiseutils.cpp
)

add_library( noiseutils-static STATIC ${libSrcs} )
target_link_libraries( noiseutils-static noise-static )

# this value is set in the root CMakeLists.txt
SET_TARGET_PROPERTIES( noiseutils-static PROPERTIES OUTPUT_NAME "noiseutils" )
include_directories("../../libnoise/1.0.0/src")

# i would like to see more projects using these defaults
if (!WIN32)
ADD_DEFINITIONS( "-Wall -ansi -pedantic -O3" )
endif()

# install libraries into /lib
INSTALL( TARGETS noiseutils-static DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
