cmake_minimum_required(VERSION 3.10)
project(Navigation VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/cmake/macros")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/cmake/macros")
include(GroupSources)

add_subdirectory(deps)
add_subdirectory(Tests)

# If building executable instead of library: cmake -DBUILD_EXECUTABLE=ON ..
option(BUILD_EXECUTABLE "Build as an executable instead of a library" OFF)

#file(GLOB SOURCE_FILES "*.cpp")
file(GLOB HEADER_FILES "*.h")

# Initialize the list of source files manually, excluding the conditional ones initially
set(SOURCE_FILES "")
file(GLOB ALL_CPP_FILES "*.cpp")

# Conditionally include source files
if(BUILD_EXECUTABLE)
    # Include all cpp files except DllMain.cpp
    foreach(FILE ${ALL_CPP_FILES})
        if(NOT FILE MATCHES "DllMain\\.cpp$")
            list(APPEND SOURCE_FILES ${FILE})
        endif()
    endforeach()
    add_executable(Navigation ${SOURCE_FILES} ${HEADER_FILES})
else()
    # Include all cpp files except Pathing.cpp
    foreach(FILE ${ALL_CPP_FILES})
        if(NOT FILE MATCHES "Pathing\\.cpp$")
            # Skip DllMain as well since static library...
            if(NOT FILE MATCHES "DllMain\\.cpp$")
                list(APPEND SOURCE_FILES ${FILE})
            endif()
        endif()
    endforeach()
    #add_library(Navigation SHARED ${SOURCE_FILES} ${HEADER_FILES})
	# Create a static library
	add_library(Navigation STATIC ${SOURCE_FILES} ${HEADER_FILES})
endif()

#add_library(Navigation SHARED ${SOURCE_FILES} ${HEADER_FILES})
#add_executable(Navigation ${SOURCE_FILES} ${HEADER_FILES})

# MSVC Compiler settings
#include(${CMAKE_CURRENT_SOURCE_DIR}/src/cmake/macros/CheckPlatform.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/src/cmake/macros/CheckPlatform.cmake)

target_compile_options(Navigation PRIVATE
    $<$<CONFIG:DEBUG>:-O0 -g>
    $<$<CONFIG:RELEASE>:-O3>
)
target_compile_definitions(Navigation PRIVATE
    $<$<CONFIG:DEBUG>:DEBUG>
    $<$<CONFIG:RELEASE>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:WIN64;_CONSOLE;_LIB;USE_STANDARD_MALLOC;PREPARED_SLN;_WINDOWS;_WIN32;_CRT_SECURE_NO_WARNINGS;DT_POLYREF64>
)

# IMPORTANT
if(UNIX AND NOT APPLE)
    target_compile_definitions(Navigation PRIVATE DT_POLYREF64)
endif()

target_include_directories(Navigation PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/fmt/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/g3dlite/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/recastnavigation/Detour/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/recastnavigation/Recast/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib"
)

target_link_libraries(Navigation
  PRIVATE
    Detour
	Recast
	fmt
	g3dlib
	#zlib
)

if(WIN32)
	set(ZLIB_LIB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib.lib")
	set(ZLIB_LIB_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib.lib")

	#add_custom_command(TARGET Navigation POST_BUILD
	#	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/build/deps/zlib/$<CONFIG>"
	#	COMMAND ${CMAKE_COMMAND} -E copy_if_different 
	#		"$<$<CONFIG:DEBUG>:${ZLIB_LIB_DEBUG}>$<$<CONFIG:RELEASE>:${ZLIB_LIB_RELEASE}>" 
	#		"${CMAKE_CURRENT_SOURCE_DIR}/build/deps/zlib/$<CONFIG>"
	#	COMMENT "Copying zlib.lib to build directory"
	#)
	add_custom_command(
    TARGET Navigation PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/build/deps/zlib/$<CONFIG>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "$<$<CONFIG:DEBUG>:${ZLIB_LIB_DEBUG}>$<$<CONFIG:RELEASE>:${ZLIB_LIB_RELEASE}>"
        "${CMAKE_CURRENT_SOURCE_DIR}/build/deps/zlib/$<CONFIG>"
    COMMENT "Copying zlib.lib to build directory before build"
	)

endif()
