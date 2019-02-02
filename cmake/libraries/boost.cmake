# FindBoost
# NOTE: FindBoost.cmake taken patched from https://gitlab.kitware.com/cmake/cmake/issues/17575
# --------------------------------------------------------------------------------------------
#
# Find Boost include dirs and libraries
#
# Use this module by invoking find_package with the form::
#
#   find_package(Boost
#     [version] [EXACT]      # Minimum or EXACT version e.g. 1.36.0
#     [REQUIRED]             # Fail with error if Boost is not found
#     [COMPONENTS <libs>...] # Boost libraries by their canonical name
#     )                      # e.g. "date_time" for "libboost_date_time"
#
# This module finds headers and requested component libraries OR a CMake
# package configuration file provided by a "Boost CMake" build.  For the
# latter case skip to the "Boost CMake" section below.  For the former
# case results are reported in variables::
#
#   Boost_FOUND            - True if headers and requested libraries were found
#   Boost_INCLUDE_DIRS     - Boost include directories
#   Boost_LIBRARY_DIRS     - Link directories for Boost libraries
#   Boost_LIBRARIES        - Boost component libraries to be linked
#   Boost_<C>_FOUND        - True if component <C> was found (<C> is upper-case)
#   Boost_<C>_LIBRARY      - Libraries to link for component <C> (may include
#                            target_link_libraries debug/optimized keywords)
#   Boost_VERSION          - BOOST_VERSION value from boost/version.hpp
#   Boost_LIB_VERSION      - Version string appended to library filenames
#   Boost_MAJOR_VERSION    - Boost major version number (X in X.y.z)
#   Boost_MINOR_VERSION    - Boost minor version number (Y in x.Y.z)
#   Boost_SUBMINOR_VERSION - Boost subminor version number (Z in x.y.Z)
#   Boost_LIB_DIAGNOSTIC_DEFINITIONS (Windows)
#                          - Pass to add_definitions() to have diagnostic
#                            information about Boost's automatic linking
#                            displayed during compilation
#
# This module reads hints about search locations from variables::
#
#   BOOST_ROOT             - Preferred installation prefix
#    (or BOOSTROOT)
#   BOOST_INCLUDEDIR       - Preferred include directory e.g. <prefix>/include
#   BOOST_LIBRARYDIR       - Preferred library directory e.g. <prefix>/lib
#   Boost_NO_SYSTEM_PATHS  - Set to ON to disable searching in locations not
#                            specified by these hint variables. Default is OFF.
#   Boost_ADDITIONAL_VERSIONS
#                          - List of Boost versions not known to this module
#                            (Boost install locations may contain the version)
#
# and saves search results persistently in CMake cache entries::
#
#   Boost_INCLUDE_DIR         - Directory containing Boost headers
#   Boost_LIBRARY_DIR_RELEASE - Directory containing release Boost libraries
#   Boost_LIBRARY_DIR_DEBUG   - Directory containing debug Boost libraries
#   Boost_<C>_LIBRARY_DEBUG   - Component <C> library debug variant
#   Boost_<C>_LIBRARY_RELEASE - Component <C> library release variant
#
# The following :prop_tgt:`IMPORTED` targets are also defined::
#
#   Boost::boost                  - Target for header-only dependencies
#                                   (Boost include directory)
#   Boost::<C>                    - Target for specific component dependency
#                                   (shared or static library); <C> is lower-
#                                   case
#   Boost::diagnostic_definitions - interface target to enable diagnostic
#                                   information about Boost's automatic linking
#                                   during compilation (adds BOOST_LIB_DIAGNOSTIC)
#   Boost::disable_autolinking    - interface target to disable automatic
#                                   linking with MSVC (adds BOOST_ALL_NO_LIB)
#   Boost::dynamic_linking        - interface target to enable dynamic linking
#                                   linking with MSVC (adds BOOST_ALL_DYN_LINK)
#
# Implicit dependencies such as Boost::filesystem requiring
# Boost::system will be automatically detected and satisfied, even
# if system is not specified when using find_package and if
# Boost::system is not added to target_link_libraries.  If using
# Boost::thread, then Threads::Threads will also be added automatically.
#
# It is important to note that the imported targets behave differently
# than variables created by this module: multiple calls to
# find_package(Boost) in the same directory or sub-directories with
# different options (e.g. static or shared) will not override the
# values of the targets created by the first call.
#
# Users may set these hints or results as cache entries.  Projects
# should not read these entries directly but instead use the above
# result variables.  Note that some hint names start in upper-case
# "BOOST".  One may specify these as environment variables if they are
# not specified as CMake variables or cache entries.
#
# This module first searches for the Boost header files using the above
# hint variables (excluding BOOST_LIBRARYDIR) and saves the result in
# Boost_INCLUDE_DIR.  Then it searches for requested component libraries
# using the above hints (excluding BOOST_INCLUDEDIR and
# Boost_ADDITIONAL_VERSIONS), "lib" directories near Boost_INCLUDE_DIR,
# and the library name configuration settings below.  It saves the
# library directories in Boost_LIBRARY_DIR_DEBUG and
# Boost_LIBRARY_DIR_RELEASE and individual library
# locations in Boost_<C>_LIBRARY_DEBUG and Boost_<C>_LIBRARY_RELEASE.
# When one changes settings used by previous searches in the same build
# tree (excluding environment variables) this module discards previous
# search results affected by the changes and searches again.
#
# Boost libraries come in many variants encoded in their file name.
# Users or projects may tell this module which variant to find by
# setting variables::
#
#   Boost_USE_DEBUG_LIBS     - Set to ON or OFF to specify whether to search
#                              and use the debug libraries.  Default is ON.
#   Boost_USE_RELEASE_LIBS   - Set to ON or OFF to specify whether to search
#                              and use the release libraries.  Default is ON.
#   Boost_USE_MULTITHREADED  - Set to OFF to use the non-multithreaded
#                              libraries ('mt' tag).  Default is ON.
#   Boost_USE_STATIC_LIBS    - Set to ON to force the use of the static
#                              libraries.  Default is OFF.
#   Boost_USE_STATIC_RUNTIME - Set to ON or OFF to specify whether to use
#                              libraries linked statically to the C++ runtime
#                              ('s' tag).  Default is platform dependent.
#   Boost_USE_DEBUG_RUNTIME  - Set to ON or OFF to specify whether to use
#                              libraries linked to the MS debug C++ runtime
#                              ('g' tag).  Default is ON.
#   Boost_USE_DEBUG_PYTHON   - Set to ON to use libraries compiled with a
#                              debug Python build ('y' tag). Default is OFF.
#   Boost_USE_STLPORT        - Set to ON to use libraries compiled with
#                              STLPort ('p' tag).  Default is OFF.
#   Boost_USE_STLPORT_DEPRECATED_NATIVE_IOSTREAMS
#                            - Set to ON to use libraries compiled with
#                              STLPort deprecated "native iostreams"
#                              ('n' tag).  Default is OFF.
#   Boost_COMPILER           - Set to the compiler-specific library suffix
#                              (e.g. "-gcc43").  Default is auto-computed
#                              for the C++ compiler in use.  A list may be
#                              used if multiple compatible suffixes should
#                              be tested for, in decreasing order of
#                              preference.
#   Boost_THREADAPI          - Suffix for "thread" component library name,
#                              such as "pthread" or "win32".  Names with
#                              and without this suffix will both be tried.
#   Boost_NAMESPACE          - Alternate namespace used to build boost with
#                              e.g. if set to "myboost", will search for
#                              myboost_thread instead of boost_thread.
#
# Other variables one may set to control this module are::
#
#   Boost_DEBUG              - Set to ON to enable debug output from FindBoost.
#                              Please enable this before filing any bug report.
#   Boost_DETAILED_FAILURE_MSG
#                            - Set to ON to add detailed information to the
#                              failure message even when the REQUIRED option
#                              is not given to the find_package call.
#   Boost_REALPATH           - Set to ON to resolve symlinks for discovered
#                              libraries to assist with packaging.  For example,
#                              the "system" component library may be resolved to
#                              "/usr/lib/libboost_system.so.1.42.0" instead of
#                              "/usr/lib/libboost_system.so".  This does not
#                              affect linking and should not be enabled unless
#                              the user needs this information.
#   Boost_LIBRARY_DIR        - Default value for Boost_LIBRARY_DIR_RELEASE and
#                              Boost_LIBRARY_DIR_DEBUG.
#
# On Visual Studio and Borland compilers Boost headers request automatic
# linking to corresponding libraries.  This requires matching libraries
# to be linked explicitly or available in the link library search path.
# In this case setting Boost_USE_STATIC_LIBS to OFF may not achieve
# dynamic linking.  Boost automatic linking typically requests static
# libraries with a few exceptions (such as Boost.Python).  Use::
#
#   add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
#
# to ask Boost to report information about automatic linking requests.
#
# Example to find Boost headers only::
#
#   find_package(Boost 1.36.0)
#   if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#     add_executable(foo foo.cc)
#   endif()
#
# Example to find Boost libraries and use imported targets::
#
#   find_package(Boost 1.56 REQUIRED COMPONENTS
#                date_time filesystem iostreams)
#   add_executable(foo foo.cc)
#   target_link_libraries(foo Boost::date_time Boost::filesystem
#                             Boost::iostreams)
#
# Example to find Boost headers and some *static* (release only) libraries::
#
#   set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
#   set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
#   set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
#   set(Boost_USE_MULTITHREADED      ON)
#   set(Boost_USE_STATIC_RUNTIME    OFF)
#   find_package(Boost 1.36.0 COMPONENTS date_time filesystem system ...)
#   if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#     add_executable(foo foo.cc)
#     target_link_libraries(foo ${Boost_LIBRARIES})
#   endif()
#
# Boost CMake
# ^^^^^^^^^^^
#
# If Boost was built using the boost-cmake project it provides a package
# configuration file for use with find_package's Config mode.  This
# module looks for the package configuration file called
# BoostConfig.cmake or boost-config.cmake and stores the result in cache
# entry "Boost_DIR".  If found, the package configuration file is loaded
# and this module returns with no further action.  See documentation of
# the Boost CMake package configuration for details on what it provides.
#
# Set Boost_NO_BOOST_CMAKE to ON to disable the search for boost-cmake.

message("\nBOOST.CMAKE")

set( BOOST_ROOT "/usr/local")
message( "BOOST_ROOT:${BOOST_ROOT}" )
set( Boost_LIBRARY_DIRS "/usr/local/lib")
message( "BOOST_LIBRARY_DIRS:${Boost_LIBRARY_DIRS}" )
set( Boost_LIBRARY_DIR "/usr/local/lib")
message( "BOOST_LIBRARY_DIR:${Boost_LIBRARY_DIR}" )
set( BOOST_LIBRARYDIR "/usr/local/lib")
message( "BOOST_LIBRARYDIR:${BOOST_LIBRARYDIR}" )
set(BOOST_INCLUDEDIR "/usr/local/include")
message("BOOST_INCLUDEDIR:${BOOST_INCLUDEDIR}")
set(_arch_suffix 64)
message("_arch_suffix:${_arch_suffix}")

#Options for the finder
set(Boost_DEBUG 1)
set(Boost_DETAILED_FAILURE_MSG 1)


# Script for compiler switching
if( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
    message( "USING GCC " ${CMAKE_CXX_COMPILER} )
    if(CMAKE_CXX_COMPILER MATCHES "g\\+\\+\\-7")
        message( "USING GCC 7" )
        set( Boost_COMPILER "-gcc73" "gcc73" )
    else()
        set( Boost_COMPILER "-gcc" "gcc" )
    endif()
elseif( CMAKE_CXX_COMPILER_ID MATCHES "clang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    message( "USING CLANG" )
    message("${CMAKE_EXE_LINKER_FLAGS}")
    if( CMAKE_EXE_LINKER_FLAGS MATCHES "lc\\+\\+" )
        message( "USING LIBC++" )
        set(Boost_COMPILER "-clanglibc++" "clanglibc++" )
    else()
        set(Boost_COMPILER "-clang60" "clang60" )
    endif()
else()
    message( WARNING "DID NOT MATCH A COMPILER!")
endif()

message( "Setting Boost options" )
set( Boost_DETAILED_FAILURE_MSG ON )
set( Boost_USE_STATIC_LIBS OFF ) # enable dynamic linking
set( Boost_USE_STATIC_RUNTIME OFF )
set (Boost_USE_MULTITHREAD ON)  # enable multithreading


#find_package(Boost)
find_package(Boost COMPONENTS REQUIRED filesystem graph regex thread unit_test_framework )

if (Boost_FOUND)
    include_directories(SYSTEM ${Boost_INCLUDE_DIR})
    message("COMPILER:${Boost_COMPILER}")

    string(STRIP "${Boost_LIBRARIES}" Boost_LIBRARIES )
    if( NOT PROJECT_LINK_LIBS OR $PROJECT_LINK_LIBS STREQUAL "" )
        set( PROJECT_LINK_LIBS "${Boost_LIBRARIES}" )
        message( "Boost:previously empty PROJECT_LINK_LIBS: ${PROJECT_LINK_LIBS}" )
    else()
        set( PROJECT_LINK_LIBS "${PROJECT_LINK_LIBS}" "${Boost_LIBRARIES}" )
        message( "Boost:previously nonempty PROJECT_LINK_LIBS: ${PROJECT_LINK_LIBS}" )
    endif()
else()
    message("ERROR:BOOST NOT FOUND!")
    message( "INCLUDES:${Boost_INCLUDE_DIRS}" )
    message( "LIBS:${Boost_LIBRARY_DIRS}" )
endif()

