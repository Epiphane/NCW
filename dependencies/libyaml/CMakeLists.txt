# Add libyaml
set(VERSION "0.2.2")

if (WIN32)
add_definitions("/W0")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}")
list(APPEND CW_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}/include")
list(APPEND CW_LIBS yaml)
set_target_properties(example-deconstructor PROPERTIES FOLDER "_")
set_target_properties(example-deconstructor-alt PROPERTIES FOLDER "_")
set_target_properties(example-reformatter PROPERTIES FOLDER "_")
set_target_properties(example-reformatter-alt PROPERTIES FOLDER "_")
set_target_properties(run-dumper run-emitter run-emitter-test-suite run-loader run-parser run-parser-test-suite run-scanner PROPERTIES FOLDER "_")
set_target_properties(test-reader PROPERTIES FOLDER "_")
set_target_properties(test-version PROPERTIES FOLDER "_")

# Propagate variables back up
set(CW_INCLUDES ${CW_INCLUDES} PARENT_SCOPE)
set(CW_LIBS ${CW_LIBS} PARENT_SCOPE)
