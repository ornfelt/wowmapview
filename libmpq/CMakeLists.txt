cmake_minimum_required(VERSION 3.10)
project(libmpq)

set(CMAKE_CXX_STANDARD 11)

# Include directories
#include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../zlib)

# Define source files for libmpq
set(LIBMPQ_SOURCES
    common.cpp
    explode.cpp
    extract.cpp
    huffman.cpp
    wave.cpp
    mpq.cpp
)

# Globbing zlib files if many and names are stable
file(GLOB ZLIB_SOURCES "zlib/*.c")

# Create the static library
add_library(mpq STATIC ${LIBMPQ_SOURCES} ${ZLIB_SOURCES})

# Create the shared library
add_library(mpq_shared SHARED ${LIBMPQ_SOURCES} ${ZLIB_SOURCES})

# If the project name is `libmpq`, you might not want `lib` prefix for shared libraries
set_target_properties(mpq_shared PROPERTIES OUTPUT_NAME "mpq")

# Set the output names for convenience
set_target_properties(mpq PROPERTIES OUTPUT_NAME "mpq")

#install(TARGETS mpq_shared
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib
#        RUNTIME DESTINATION bin)  # For Windows DLLs
