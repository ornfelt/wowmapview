cmake_minimum_required(VERSION 3.20)
project(wowmapview)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD 11)
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
endif()
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if (${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	message(STATUS "Using Linux port")
    #add_definitions(-D_LINUX)
    #add_definitions(-D_LINUX32)
	
    #add_definitions(-D_FILE_OFFSET_BITS=32)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")

    #add_definitions(-D__SYS_ZLIB)
	#add_definitions(-D_7ZIP_ST)
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
file(GLOB HEADER_FILES "*.h")

add_executable(wowmapview wowmapview.cpp areadb.cpp dbcfile.cpp font.cpp frustum.cpp liquid.cpp maptile.cpp menu.cpp model.cpp mpq_libmpq.cpp particle.cpp shaders.cpp sky.cpp test.cpp video.cpp wmo.cpp world.cpp NodeManager.cpp ${HEADER_FILES})
if(WIN32)
    #set(ZLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/zlib/")
    #set(LIBMPQ_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libmpq")
    set(SDL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/SDL/include")
    #set(SDL_LIBRARY "${CMAKE_SOURCE_DIR}/SDL/lib/")
    set(SDL_LIBRARY "SDL")
    #set(SDL_MAIN_LIBRARY "${CMAKE_SOURCE_DIR}/SDL/lib/SDLmain.lib")
	
endif()
#target_include_directories(wowmapview PUBLIC "D:/Soft/web-scrap/vcpkg/installed/x86-windows/include")
#[[file(GLOB zlibfiles zlib/*.c libmpq/*.h *.h)

add_library(zliblocal STATIC
        ${zlibfiles}
        )]]

#target_link_libraries(libmpq PRIVATE zliblocal)
#include_directories(zlib)

set(ZLIB_PUBLIC_HDRS
        ${CMAKE_SOURCE_DIR}/zlib/zconf.h
        ${CMAKE_SOURCE_DIR}/zlib/zlib.h
        )
set(ZLIB_PRIVATE_HDRS
        ${CMAKE_SOURCE_DIR}/zlib/crc32.h
        ${CMAKE_SOURCE_DIR}/zlib/deflate.h
        ${CMAKE_SOURCE_DIR}/zlib/inffast.h
        ${CMAKE_SOURCE_DIR}/zlib/inffixed.h
        ${CMAKE_SOURCE_DIR}/zlib/inflate.h
        ${CMAKE_SOURCE_DIR}/zlib/inftrees.h
        ${CMAKE_SOURCE_DIR}/zlib/trees.h
        ${CMAKE_SOURCE_DIR}/zlib/zutil.h
        )
file(GLOB zlibfiles zlib/*.c)
set(ZLIB_SRCS
        ${zlibfiles}
        )

# parse the full version number from zlib.h and include in ZLIB_FULL_VERSION
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/zlib/zlib.h _zlib_h_contents)
string(REGEX REPLACE ".*#define[ \t]+ZLIB_VERSION[ \t]+\"([-0-9A-Za-z.]+)\".*"
        "\\1" ZLIB_FULL_VERSION ${_zlib_h_contents})
#set(ZLIB_FULL_VERSION ${ZLIB_FULL_VERSION} PARENT_SCOPE)

#add_library(zlib SHARED ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
add_library(zliblocal STATIC ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
#set_target_properties(zlib PROPERTIES DEFINE_SYMBOL ZLIB_DLL)
#set_target_properties(zlib PROPERTIES SOVERSION 1)
# Creates zlib1.dll when building shared library version
#set_target_properties(zlib PROPERTIES SUFFIX "1.dll")

install(TARGETS zliblocal
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" )

# For SDL2, use find_package(SDL2 REQUIRED) and ${SDL2_INCLUDE_DIRS}
find_package(SDL REQUIRED)

#[[add_library(sdl)
set_target_properties(SDL::SDL PROPERTIES DEFINE_SYMBOL SDL_DLL)
set_target_properties(SDL::SDL PROPERTIES SOVERSION 1)
# Creates zlib1.dll when building shared library version
set_target_properties(SDL::SDL PROPERTIES SUFFIX "1.dll")

install(TARGETS sdl
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" )]]

#add_subdirectory(zlib)
#add_subdirectory(SDL/include)
#add_subdirectory(libmpq)

file(GLOB mpqfiles libmpq/*.cpp libmpq/*.h *.h)

add_library(libmpq STATIC
        ${mpqfiles}
)
# For .so
#add_library(mpq SHARED
#        ${mpqfiles}
#)

target_include_directories(libmpq PRIVATE
        zlib)
target_link_libraries(libmpq PRIVATE
        zliblocal)

add_subdirectory(./Navigation ${CMAKE_BINARY_DIR}/Navigation)

#include(${CMAKE_CURRENT_SOURCE_DIR}/Navigation/src/cmake/macros/FindMySQL.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Navigation/src/cmake/macros")

#find_package(ZLIB REQUIRED)
#include_directories(${SDL_INCLUDE_DIR})
if (NOT MSVC)
	find_package(OpenGL REQUIRED)
    find_package(MySQL REQUIRED)

    message(STATUS "Linking with MySQL libraries: ${MYSQL_LIBRARY}")
    #set(MYSQL_LIBRARIES "/usr/lib/libmariadb.so")
	
	target_link_libraries(wowmapview PRIVATE
        #ZLIB::ZLIB
        SDL
        #${CMAKE_SOURCE_DIR}/SDL/lib/SDL.lib
        #${CMAKE_SOURCE_DIR}/SDL/lib/SDLmain.lib
        zliblocal
        libmpq
		Navigation
        ${MYSQL_LIBRARY}
		)
    include_directories(${MYSQL_INCLUDE_DIR})
	
else()
	#target_link_options(wowmapview PRIVATE
    #    "/MACHINE:X86"
    #)
	
	#link_directories(
	#	${CMAKE_SOURCE_DIR}/SDL/lib
	#	"C:/Program Files/MySQL/MySQL Server 8.0/lib"
	#)
	
	target_link_libraries(wowmapview PRIVATE
        #ZLIB::ZLIB
        #SDL
		OpenGL32.lib
		GLu32.lib
        ${CMAKE_SOURCE_DIR}/SDL/lib/SDL.lib
        ${CMAKE_SOURCE_DIR}/SDL/lib/SDLmain.lib
        zliblocal
        libmpq
        Navigation
		"C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.lib"
        )
endif()

target_include_directories(wowmapview PRIVATE
        ${SDL_INCLUDE_DIR}
        zlib
        #[[${ZLIB_INCLUDE_DIRS}]]
        libmpq
        Navigation
		"C:/Program Files/MySQL/MySQL Server 8.0/include"
		)

# Linux
if(LINUX)
    # Link libraries
    target_link_libraries(wowmapview PRIVATE
        ${SDL_LIBRARY}
        ${OPENGL_LIBRARIES}
        SDL
        GL
        zliblocal
        libmpq
        #mpq_shared
		Navigation
        ${MYSQL_LIBRARIES}
    )
endif()

# Add conf file to dev folder
if(WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME}  POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/fonts/arial.info
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/arial.info)
    add_custom_command(
            TARGET ${PROJECT_NAME}  POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/fonts/arial.tga
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/arial.tga)
    add_custom_command(
            TARGET ${PROJECT_NAME}  POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/SDL/lib/SDL.dll
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SDL.dll)
    file(WRITE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/windowed_with_patch.cmd" "wowmapview -w -p")
    file(WRITE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/windowed_no_patch.cmd" "wowmapview -w -np")
endif()
