message("\nDIRS.CMAKE")

#used to set the location of the binaries to the project directory
set( CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR} )
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib )
