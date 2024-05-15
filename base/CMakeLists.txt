

add_subdirectory(./test)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
include_directories(.)

aux_source_directory(. SRC)

add_library(muduo_base ${SRC})