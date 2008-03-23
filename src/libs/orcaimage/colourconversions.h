/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMAGE_COLOURCONVERSIONS_H
#define ORCA_IMAGE_COLOURCONVERSIONS_H

#ifdef OPENCV_FOUND
#   include <cv.h>
#endif

#include <orca/camera.h>
#include <orca/image.h>

namespace orcaimage{

#ifdef OPENCV_FOUND
    //! This function has not been tested very well
    void cvtToRgb( IplImage* dest, IplImage* bayerSrc, const orca::ImageDataPtr& src );
    
    //! Often use this function for converting to bgr image format used in opencv
    //! TODO: this should only have one IplImage but imaghandler.cpp has been setup so that the bayer 
    //! IplImage is differnet to the colour IplImage... is there a way to fix this?
    //! Should this function be overloaded so that only one IplImage is an argument?
    void cvtToBgr( IplImage* dest, IplImage* bayerSrc, const orca::ImageDataPtr& src );

#endif
    
} // namespace
    
#endif
