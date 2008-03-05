SET_COMPONENT_NAMES ( SnapshotLogger )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD EXE ${APP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( BUILD EXE ${APP_NAME} NOT_OS_WIN "Windows OS is not  supported" )

SET( DEP_LIBS OrcaLog )
REQUIRE_TARGETS( BUILD EXE ${APP_NAME} ${DEP_LIBS} )

SET( DEP_LIBS ${DEP_LIBS} HydroDll OrcaIfaceImpl )
REQUIRE_VAR( BUILD EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB COMP_SRCS *.cpp )
    
    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${DEP_LIBS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        WRAP_MAINTHREAD_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${DEP_LIBS} )    
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF( BUILD )