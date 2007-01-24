/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGESERVER_COMPONENT_H
#define ORCA2_IMAGESERVER_COMPONENT_H

#include <orcaice/component.h>
#include "imagegrabber/imagegrabber.h"

namespace imageserver {

class MainLoop;
class Driver;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    MainLoop* mainLoop_;

    //
    // HARDWARE INTERFACES
    //
    Driver *hwDriver_;
    ImageGrabber* imageGrabber_; 

    // Provided interface
    Ice::ObjectPtr cameraObjPtr_;
};

}

#endif
