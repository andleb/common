message("\nTHREADS.CMAKE")

find_package( Threads REQUIRED )
set( THREADS_PREFER_PTHREAD_FLAG ON )
string(STRIP "${CMAKE_THREAD_LIBS_INIT}" CMAKE_THREAD_LIBS_INIT )
if( NOT PROJECT_LINK_LIBS OR $PROJECT_LINK_LIBS STREQUAL "" )
#    set( PROJECT_LINK_LIBS "${CMAKE_THREAD_LIBS_INIT}" )
    set( PROJECT_LINK_LIBS Threads::Threads )
    message( "Thread-empty: ${PROJECT_LINK_LIBS}" )
else()
    #10/1/2018 set thread to the front else problems with mixed format
#    set( PROJECT_LINK_LIBS "${PROJECT_LINK_LIBS} ${CMAKE_THREAD_LIBS_INIT}" )
    set( PROJECT_LINK_LIBS "${PROJECT_LINK_LIBS}" Threads::Threads )
    message( "Thread-nonempty: ${PROJECT_LINK_LIBS}" )
endif()

