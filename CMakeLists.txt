cmake_minimum_required( VERSION 3.5 )

project( common VERSION 0.1 LANGUAGES CXX )
#project( Common CXX )

include(cmake/definitions.cmake )
include(cmake/dirs.cmake)

set(HEADERS
    stack.h
    stackcontainer.h
    )

set(SOURCES
    stack.cpp
    )


add_library(common SHARED ${HEADERS} ${SOURCES})

set_target_properties(common PROPERTIES VERSION ${PROJECT_VERSION})

message( "CMAKE_CXX_COMPILER_VERSION; ${CMAKE_CXX_COMPILER_VERSION}" )
set( MAJOR "")
STRING(REGEX REPLACE "^([0-9]+)\\.+.*" "\\1" MAJOR "${CMAKE_CXX_COMPILER_VERSION}")
message( "MAJOR; ${MAJOR}" )

set( LIBSUFFIX "${CMAKE_CXX_COMPILER_ID}${MAJOR}" )

if( CMAKE_EXE_LINKER_FLAGS MATCHES "lc\\+\\+" )
    set( LIBSUFFIX "${LIBSUFFIX}libc++")
    message( "Using libc++")
endif()

string( TOLOWER ${LIBSUFFIX} LIBSUFFIX)

message( "Generated suffix; ${LIBSUFFIX}" )

#set_target_properties(common PROPERTIES SUFFIX "-${LIBSUFFIX}" )
set_target_properties(common PROPERTIES LIBRARY_OUTPUT_NAME "${PROJECT_NAME}-${LIBSUFFIX}" )
#set_target_properties(common PROPERTIES OUTPUT_NAME "${LIBRARY_OUTPUT_NAME}-${LIBSUFFIX}" )
message( "Output_Name; ${OUTPUT_NAME}" )
message( "Name; ${NAME}" )
message( "LIBRARY_OUTPUT_NAME; ${LIBRARY_OUTPUT_NAME}" )

# public api
set_target_properties(common PROPERTIES PUBLIC_HEADER "${HEADERS}" )

include(GNUInstallDirs)
set(CMAKE_INSTALL_PREFIX "/usr/local")
message( "Install prefix:" ${CMAKE_INSTALL_PREFIX})


install(TARGETS common
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" #${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common" #${CMAKE_INSTALL_INCLUDEDIR}
    )

#add_executable()
#target_link_libraries( analyzer ${PROJECT_LINK_LIBS} )
