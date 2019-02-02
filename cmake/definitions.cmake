message("\nDEFINITIONS.CMAKE")

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17" )

message( "COMPILER_ID:${CMAKE_CXX_COMPILER_ID}")
message( "STANDARD:${CMAKE_CXX_STANDARD}")
message("LINKER:${CMAKE_EXE_LINKER_FLAGS}")

if( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
    message( "USING GCC" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-macros")

# for linking against libc++ on linux
#if qtcreator sets CMAKE_CXX_COMPILER in kit selection
elseif( CMAKE_CXX_COMPILER_ID MATCHES "clang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    message( "USING CLANG" )
    set(CMAKE_CXX_COMPILER "clang++")
    # Done in IDE instead
#    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" )
#    set( CMAKE_EXE_LINKER_FLAGS "-lc++abi" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unused-macros -Wno-newline-eof\
    -Wno-exit-time-destructors -Wno-global-constructors -Wno-gnu-zero-variadic-macro-arguments\
    -Wno-documentation -Wno-shadow -Wno-missing-prototypes -Wno-vla\
    -Wno-padded" )
else()
    message( WARNING "DID NOT MATCH A COMPILER!")
    return()
endif()

message( "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
