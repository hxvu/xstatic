
#
# The following license applies only to this file:
#

# - find Lemon 3
# LEMON_INCLUDE_DIR - Where to find Lemon 3 header files (directory)
# LEMON_LIBRARIES - Lemon 3 libraries
# LEMON_LIBRARY_RELEASE - Where the release library is
# LEMON_LIBRARY_DEBUG - Where the debug library is
# LEMON_FOUND - Set to TRUE if we found everything (library, includes and executable)

# Copyright (c) 2010 Pau Garcia i Quiles, <pgquiles@elpauer.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Generated by CModuler, a CMake Module Generator - http://gitorious.org/cmoduler

IF( LEMON_INCLUDE_DIR AND LEMON_LIBRARY_RELEASE )
    SET(LEMON_FIND_QUIETLY TRUE)
ENDIF( LEMON_INCLUDE_DIR AND LEMON_LIBRARY_RELEASE )


IF ( UNIX )
   SET( LEMON_INCLUDE_DIR "/usr/local/include" )
   SET( LEMON_LIBRARY_RELEASE "/usr/local/lib" )

   FIND_PATH( LEMON_INCLUDE_DIR lemon.h  )

   FIND_LIBRARY(LEMON_LIBRARY_RELEASE NAMES lemon )

ELSE ( UNIX )

   FIND_PATH( LEMON_INCLUDE_DIR lemon.h 
              "/mingw/include" )

   FIND_LIBRARY( LEMON_LIBRARY_RELEASE NAMES lemon )

ENDIF ( UNIX )

IF( LEMON_LIBRARY_RELEASE AND LEMON_INCLUDE_DIR )
        SET( LEMON_FOUND TRUE )
ENDIF( LEMON_LIBRARY_RELEASE AND LEMON_INCLUDE_DIR )

IF( LEMON_LIBRARY_RELEASE )
        SET( LEMON_LIBRARIES "${LEMON_LIBRARY_RELEASE}/libemon.a" )
ENDIF( LEMON_LIBRARY_RELEASE )

IF( LEMON_FOUND )
        IF( NOT LEMON_FIND_QUIETLY )
                MESSAGE( STATUS "Found Lemon13 header file in ${LEMON_INCLUDE_DIR}")
                MESSAGE( STATUS "Found Lemon13 libraries: ${LEMON_LIBRARIES}")
        ENDIF( NOT LEMON_FIND_QUIETLY )
ELSE(LEMON_FOUND)
        IF( LEMON_FIND_REQUIRED)
                MESSAGE( FATAL_ERROR "Could not find Lemon13" )
        ELSE( LEMON_FIND_REQUIRED)
                MESSAGE( STATUS "Optional package Lemon13 was not found" )
        ENDIF( LEMON_FIND_REQUIRED)
		
ENDIF(LEMON_FOUND)

