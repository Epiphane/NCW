SET(PROJECT_NAME sqlite)

set(libSrcs 
   "3.22.0/sqlite3ext.h"
   "3.22.0/sqlite3.h"
   "3.22.0/sqlite3.c"
   "3.22.0/shell.c"
)

add_library( sqlite STATIC ${libSrcs} )

# this value is set in the root CMakeLists.txt
SET_TARGET_PROPERTIES( sqlite PROPERTIES OUTPUT_NAME "sqlite" )

# i would like to see more projects using these defaults
ADD_DEFINITIONS( "-Wall -ansi -pedantic -O3" )

# install libraries into /lib
INSTALL( TARGETS sqlite DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
