ORCA_SET_LIBRARY_NAME( OrcaVfh )
# originally a fork of Player code (Player is GPL)
GBX_ADD_LICENSE( GPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs OrcaLocalNav )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )

    target_link_libraries( ${LIB_NAME} ${dep_libs} )

#    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

endif( build )
