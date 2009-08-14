ORCA_SET_LIBRARY_NAME( HydroImageOcvUtil )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

# make sure OpenCV has been installed
include(${HYDRO_CMAKE_DIR}/FindOpenCV.cmake)
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} OPENCV_FOUND "OpenCv not found" )
include_directories( ${OPENCV_INCLUDE_DIRS} )

include(${HYDRO_CMAKE_DIR}/FindDc1394.cmake)
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} DC1394_FOUND "libdc1394v2 (dc1394 ver 2) was not found" )

set( dep_libs ${OPENCV_LIBS} dc1394 raw1394  )

if( build )

    include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    INCLUDE (${HYDRO_CMAKE_DIR}/UseOpenCV.cmake)
    
    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( hydroimageocvutil ${hdrs} )
    
    #if( ORCA_BUILD_TESTS )
    #    add_subdirectory( test )
    #endif( ORCA_BUILD_TESTS )

endif( build )