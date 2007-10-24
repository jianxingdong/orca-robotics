# Locate Orca home

# This module defines
# ORCA_HOME where to find include, lib, bin, etc.
# ORCA_FOUND, If set to 0, don't try to use Orca.

# start with 'not found'
SET( ORCA_FOUND 0 CACHE BOOL "Do we have Orca?" )

FIND_PATH( ORCA_HOME orca_manifest.h
  $ENV{ORCA_HOME}
  # Test standard installation points
  /opt/orca
  /opt/orca-2.7.0+
  /opt/orca-2.7.0
  /opt/orca-2.6.0+
  /opt/orca-2.6.0
  /opt/orca-2.5.0
  /opt/orca-2.4.0
  /opt/orca-2.3.0
  /opt/orca-2.2.0
  /opt/orca-2.2.0
  /opt/orca-2.1.0
  /opt/orca-2.0.0
  C:/orca
  )
# MESSAGE( STATUS "DEBUG: orca_manifest.cmake is apparently found in : ${ORCA_HOME}" )

# NOTE: if ORCA_HOME_INCLUDE_ORCA is set to *-NOTFOUND it will evaluate to FALSE
IF ( ORCA_HOME )
    SET( ORCA_FOUND 1 CACHE BOOL "Do we have Orca?" FORCE )
ENDIF ( ORCA_HOME )

# do we need it in cache?
# SET( ORCA_HOME ${ORCA_HOME} CACHE PATH "Orca installed directory" FORCE )
#   MESSAGE( "Setting Orca installed directory to ${ORCA_HOME}" )