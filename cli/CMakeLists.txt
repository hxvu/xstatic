cmake_minimum_required(VERSION 2.8)
project(CodeQueryCLI)

#add_definitions( -Wall )

find_package(Sqlite REQUIRED)
#find_package(Lemon REQUIRED)

include_directories( "." )
include_directories( "../makedb" )
include_directories( "../querylib" )
include_directories( ${SQLITE_INCLUDE_DIR} )
#include_directories( ${LEMON_INCLUDE_DIR} )
#include_directories( "/usr/local/include/lemon" )

set( CQSEARCH_SRCS
     main_cli.cpp
   )

add_executable( cqsearch ${CQSEARCH_SRCS} )
#target_link_libraries( cqsearch ${SQLITE_LIBRARIES} ${LEMON_LIBRARIES} small_lib optlist_lib sqlquery_lib )
target_link_libraries( cqsearch ${SQLITE_LIBRARIES} small_lib optlist_lib sqlquery_lib )

install(TARGETS cqsearch RUNTIME DESTINATION bin)


