ORCA_SET_COMPONENT_NAME( MultiCameraServer )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )

set( dep_libs HydroDll HydroInterfaces HydroImage OrcaIfaceImpl OrcaObjects OrcaImage )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build)

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    file( GLOB srcs *.cpp )

    # build stand-alone component  
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    if( ORCA_BUILD_SERVICES )
        ORCA_WRAP_MAINTHREAD_INTO_SERVICE( ${srcs} )
        target_link_libraries( ${SERVICE_NAME} ${dep_libs} )    
    endif( ORCA_BUILD_SERVICES )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
