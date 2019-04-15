# Add rapidjson
set(VERSION "1.1.0")

list(APPEND CW_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/${VERSION}")

# Propagate variables back up
set(CW_INCLUDES ${CW_INCLUDES} PARENT_SCOPE)
