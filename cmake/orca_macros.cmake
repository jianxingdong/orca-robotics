# macros for the Orca project

#
# Components should add themselves by calling 'GLOBAL_ADD_EXECUTABLE' 
# instead of 'ADD_EXECUTABLE' in CMakeLists.txt.
#
# This gives a centralised location where all components are registered
# and lets us add various things to all components in just one place.
#
# Usage: GLOBAL_ADD_EXECUTABLE( NAME src1 src2 src3 )
#
MACRO( GLOBAL_ADD_EXECUTABLE NAME )
  ADD_EXECUTABLE( ${NAME} ${ARGN} )
  INSTALL_TARGETS( /bin ${NAME} )
  SET ( L ${COMPONENT_LIST} )
  LIST ( APPEND L ${NAME} )
#   MESSAGE ( STATUS "DEBUG: ${L}" )
  SET ( COMPONENT_LIST ${L} CACHE STRING "Global list of components to build" FORCE )
  MESSAGE( STATUS "Planning to Build Executable: ${NAME}" )
ENDMACRO( GLOBAL_ADD_EXECUTABLE NAME )

#
# Components should add themselves by calling 'GLOBAL_ADD_EXECUTABLE' 
# instead of 'ADD_LIBRARY' in CMakeLists.txt.
#
# This gives a centralised location where all components are registered
# and lets us add various things to all components in just one place.
#
# Usage: GLOBAL_ADD_LIBRARY( NAME src1 src2 src3 )
#
MACRO( GLOBAL_ADD_LIBRARY NAME )
  ADD_LIBRARY( ${NAME} ${ARGN} )
  INSTALL_TARGETS( /lib ${NAME} )
  SET ( L ${LIBRARY_LIST} )
  LIST ( APPEND L ${NAME} )
#  MESSAGE ( STATUS "DEBUG: ${L}" )
  SET ( LIBRARY_LIST ${L} CACHE STRING "Global list of libraries to build" FORCE )
  MESSAGE( STATUS "Planning to Build Library   : ${NAME}" )
ENDMACRO( GLOBAL_ADD_LIBRARY NAME )

#
# This is a mechanism to register special items which are not
# components or libraries. This function only records the name of
# the item to display it at the end of the cmake run and to submit
# to the Dashboard.
#
# Usage: GLOBAL_ADD_ITEM( NAME )
#
MACRO( GLOBAL_ADD_ITEM NAME )
  SET ( L ${ITEM_LIST} )
  LIST ( APPEND L ${NAME} )
#  MESSAGE ( STATUS "DEBUG: ${L}" )
  SET ( ITEM_LIST ${L} CACHE STRING "Global list of special items to build" FORCE )
  MESSAGE( STATUS "Planning to Build Item      : ${NAME}" )
ENDMACRO( GLOBAL_ADD_ITEM NAME )

#
# Macro to check for optional sub-libraries, eg in the case where
# a single component can drive various bits of hardware with the 
# same interface.
#
# It's probably just as easy to write this stuff by hand, but
# using a macro standardizes the trace statements.
#
# USAGE: OPTIONAL_SUB_LIBRARY( 
#                   DESCRIPTION 
#                   SUBDIRECTORY 
#                   OUTPUT_LIBRARY 
#                   LINK_LIBS
#                   OK_TO_BUILD 
#                   DEFINITION_TAG
#                   lib1 lib2 ... libn )
#
# Where:
#  - DESCRIPTION:       Descriptive message
#  - SUBDIRECTORY:      Where the code lives
#  - OUTPUT_LIBRARY:    The optional sub-library
#  - LINK_LIBS:         Extra libraries one needs to link against
#  - OK_TO_BUILD:       The name of the variable that holds whether this sub-library be build
#  - DEFINITION_TAG:    String tag used for both: the compiler flag -Dxxx and the CMake variable.
#                       Both indicate the library can be built.
#  - lib1 ... libn:     The extra libraries that need to be linked against the component
#
MACRO( OPTIONAL_SUB_LIBRARY DESCRIPTION SUBDIRECTORY OUTPUT_LIBRARY LINK_LIBS OK_TO_BUILD DEFINITION_TAG )

  IF( ${${OK_TO_BUILD}} )
    MESSAGE( STATUS "    ${DESCRIPTION} - can be built")
    ADD_SUBDIRECTORY ( ${SUBDIRECTORY} )
    
    # The top level executable will be linked to this optional libraries...
    SET( SUB_LINK_LIBRARIES ${OUTPUT_LIBRARY} )
    # ... and all the libraries it depends on.
    FOREACH( ARG ${ARGN} )
        SET( SUB_LINK_LIBRARIES ${SUB_LINK_LIBRARIES} ${ARG} )
    ENDFOREACH( ARG ${ARGN} )
    SET( ${LINK_LIBS}  ${SUB_LINK_LIBRARIES} )
    
    LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/${SUBDIRECTORY} )
    ADD_DEFINITIONS( -D${DEFINITION_TAG} )
    SET(${DEFINITION_TAG} TRUE)
#    ADD_LIBRARY( ${OUTPUT_LIBRARY} )
  ELSE(  ${${OK_TO_BUILD}} )
    MESSAGE( STATUS "    ${DESCRIPTION} - CANNOT be built")
    SET(${DEFINITION_TAG} FALSE)
  ENDIF( ${${OK_TO_BUILD}} )

ENDMACRO( OPTIONAL_SUB_LIBRARY DESCRIPTION DIRECTORY LIBNAME )


# BUILD_COMPONENT_SOURCES
#   DEPRICATED!!!! Not portable!!!
#   Builds source files common to stand-alone component and IceStorm service.
#
#       BUILD_COMPONENT_SOURCES( CONV_LIB_NAME )
#   The common sources are compiled into a convenience static library with
#   specified name.
#
MACRO( BUILD_COMPONENT_SOURCES CONV_LIB_NAME )
    # "common sources" are common to stand-alone component and IceStorm service 
    # these include ALL .cpp files EXCEPT main.cpp and service.cpp
    FILE( GLOB COMMON_SRCS *.cpp )
    LIST( REMOVE_ITEM COMMON_SRCS 
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" 
        "${CMAKE_CURRENT_SOURCE_DIR}/service.cpp" )

    # build common sources into a static "convenience" library
    ADD_LIBRARY ( ${CONV_LIB_NAME} ${COMMON_SRCS} )
    # special dependency on the generated files 
    # (each project should wait for its own interface lib)
    ADD_DEPENDENCIES( ${CONV_LIB_NAME} ${PROJECT_INTERFACE_LIB} )
ENDMACRO( BUILD_COMPONENT_SOURCES CONV_LIB_NAME )

# FIND_COMPONENT_SOURCES
#   Returns a list of source files except main.cpp and service.cpp
#
#       FIND_COMPONENT_SOURCES( VAR_NAME )
#
MACRO( FIND_COMPONENT_SOURCES VAR_NAME )
    # "common sources" are common to stand-alone component and IceStorm service 
    # these include ALL .cpp files EXCEPT main.cpp and service.cpp
    FILE( GLOB COMMON_SRCS *.cpp )
    LIST( REMOVE_ITEM COMMON_SRCS 
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" 
        "${CMAKE_CURRENT_SOURCE_DIR}/service.cpp" )

    SET( ${VAR_NAME} ${COMMON_SRCS} )
ENDMACRO( FIND_COMPONENT_SOURCES VAR_NAME )

#
# GENERATE_CONFIG_FILE
#   Rule for generating .cfg files from .def files.
#
#       GENERATE_CONFIG_FILE( DEF_FILE [COMPONENT_TARGET='DEF_FILE_WE'] [INSTALL_CFG=1] )
#
#   For .def file mycomponent.def, generates file mycomponent.cfg
#   If target name is not specified, it is assumed to be mycomponent.
#   Generated config files are installed by default, specify INSTALL_CFG=0 if installation
#   is not required (useful for config files used in tests).
#
MACRO( GENERATE_CONFIG_FILE DEF_FILE )

    IF ( ${ARGC} LESS 1 )
        MESSAGE( FATAL_ERROR "GENERATE_CONFIG_FILE requires at least one input parameter." )
    ENDIF ( ${ARGC} LESS 1 )
    
    IF ( ${ARGC} GREATER 1 )
        # Target name is specified
        SET ( COMPONENT_TARGET ${ARGV1} )
    ELSE ( ${ARGC} GREATER 1 )
        # Target name is assumed from the def file name
        GET_FILENAME_COMPONENT( COMPONENT_TARGET ${DEF_FILE} NAME_WE )
    #     MESSAGE ( STATUS "DEBUG: assumed component target name ${COMPONENT_TARGET}" )
    ENDIF ( ${ARGC} GREATER 1 )
    
    IF ( ${ARGC} GREATER 2 )
        # Target name is specified
        SET ( INSTALL_DEF ${ARGV2} )
        SET ( INSTALL_CFG ${ARGV2} )
        SET ( INSTALL_XML ${ARGV2} )
    ELSE ( ${ARGC} GREATER 2 )
        # Default behavior is to install the config file
        SET ( INSTALL_DEF 1 )
        SET ( INSTALL_CFG 1 )
        SET ( INSTALL_XML 1 )
    #     MESSAGE ( STATUS "DEBUG: assumed that the config file needs to be installed" )
    ENDIF ( ${ARGC} GREATER 2 )
    
    IF ( INSTALL_DEF )
        INSTALL_FILES( /def FILES ${CMAKE_CURRENT_SOURCE_DIR}/${DEF_FILE} )
    ENDIF ( INSTALL_DEF )
    
    # The standard CMAKE_CFG_INTDIR does not seem to get resolved in INSTALL_FILES, do it manually
    # alexm: I'm not sure if this is the right way to do it. I think this is a problem only 
    # when trying to install from command line. so maybe not all the variables are set.
    IF( OS_WIN )
        # VCC defaults to Debug
        SET( MANUAL_CFG_INTDIR "debug" )
    ELSE( OS_WIN )
        SET( MANUAL_CFG_INTDIR "." )
    ENDIF( OS_WIN )
    
    STRING( REGEX REPLACE "\\.def" ".cfg" CFG_FILE ${DEF_FILE} )
    ADD_CUSTOM_COMMAND( 
        TARGET ${COMPONENT_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "-- Generating ${CFG_FILE} from ${DEF_FILE}"
        COMMAND ${ORCA_DEF2CFG_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${DEF_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${CFG_FILE}
        # doesn't seem to work for some reason
    #     COMMENT "-- Generating ${CFG_FILE} from ${DEF_FILE}"
        )
    
    IF ( ORCA_MOTHERSHIP )
        ADD_DEPENDENCIES( ${COMPONENT_TARGET} def2cfg def2xml def2xmltemplate )
    ENDIF ( ORCA_MOTHERSHIP )
    
    IF ( INSTALL_CFG )
        INSTALL_FILES( /cfg FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${CFG_FILE} )
    ENDIF ( INSTALL_CFG )
    
    # this is a global option
    IF ( GENERATE_XML )
        #
        # ICEGRID APPLICATION DESCRIPTOR XML FILE
        #
        STRING( REGEX REPLACE "\\.def" ".xml" XML_FILE ${DEF_FILE} )
        ADD_CUSTOM_COMMAND( 
            TARGET ${COMPONENT_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "-- Generating ${XML_FILE} from ${DEF_FILE}"
            COMMAND ${ORCA_DEF2XML_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${DEF_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${XML_FILE}
            # doesn't seem to work for some reason
        #     COMMENT "-- Generating ${XML_FILE} from ${DEF_FILE}"
            )
        
        IF ( INSTALL_XML )
            INSTALL_FILES( /xml FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${XML_FILE} )
        ENDIF ( INSTALL_XML )
        
        #
        # ICEGRID SERVER TEMPLATE XML FILE
        #
        STRING( REGEX REPLACE "\\.def" "-template.xml" TEMPLATE_XML_FILE ${DEF_FILE} )
        ADD_CUSTOM_COMMAND( 
            TARGET ${COMPONENT_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "-- Generating ${TEMPLATE_XML_FILE} from ${DEF_FILE}"
            COMMAND ${ORCA_DEF2XMLTEMPLATE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${DEF_FILE} 
                ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${TEMPLATE_XML_FILE}
            # doesn't seem to work for some reason
        #     COMMENT "-- Generating ${XML_FILE} from ${DEF_FILE}"
            )
        
        IF ( INSTALL_XML )
            INSTALL_FILES( /xml FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${TEMPLATE_XML_FILE} )
        ENDIF ( INSTALL_XML )
    ENDIF ( GENERATE_XML )

ENDMACRO( GENERATE_CONFIG_FILE DEF_FILE )


#
# ASSERT( TEST COMMENT_FAIL [COMMENT_PASS=''] [IS_FATAL=FALSE] )
#
MACRO( ASSERT TEST COMMENT_FAIL )
# MESSAGE( STATUS "DEBUG: number of inputs : ${ARGC}" )
# FOREACH( argi ${ARGV} )
#     MESSAGE( STATUS "DEBUG: input i : ${argi}" )
# ENDFOREACH( argi )

    IF ( ${TEST} )
#         MESSAGE( STATUS "DEBUG: assertion passed : ${TEST}" )

        # ARG2 holds COMMENT_PASS
        IF ( ${ARGC} GREATER 2 )
            MESSAGE( STATUS ${ARGV2} )
        ENDIF ( ${ARGC} GREATER 2 )

    ELSE ( ${TEST} )
#         MESSAGE( STATUS "DEBUG: assertion failed : ${TEST}" )

        SET ( IS_FATAL 0 )
        IF ( ${ARGC} GREATER 3 )
            SET ( IS_FATAL ${ARGV3} )
        ENDIF ( ${ARGC} GREATER 3 )

        IF ( ${IS_FATAL} )
#             MESSAGE( STATUS "DEBUG: failure is fatal : ${IS_FATAL}" )
            MESSAGE( FATAL_ERROR ${COMMENT_FAIL} )
        ELSE ( ${IS_FATAL} )
#             MESSAGE( STATUS "DEBUG: failure is NOT fatal : ${IS_FATAL}" )
            MESSAGE( STATUS ${COMMENT_FAIL} )
        ENDIF ( ${IS_FATAL} )

    ENDIF ( ${TEST} )

ENDMACRO( ASSERT )
