/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS_MAINLOOP_H
#define ORCA2_GPS_MAINLOOP_H

#include <orca/gps.h>
#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>
#include <orcagpsutil/mapgrid.h>

#include "driver.h"
#include "gpsinterfaces.h"

namespace gps {

//
// @brief the main loop of this GPS component.
//
// Note: this thing self-destructs when run() returns.
//
class MainLoop : public orcaice::Thread
{

public:

    MainLoop( GpsIfacePtr         &gpsInterface,
              GpsMapGridIfacePtr  &gpsMapGridInterface,
              GpsTimeIfacePtr     &gpsTimeInterface,
              Driver              *hwDriver,
              const orca::Frame3d &antennaOffset,
              orcaice::Context     current );
    ~MainLoop();

    virtual void run();

private:

    GpsIfacePtr        &gpsInterface_;
    GpsMapGridIfacePtr &gpsMapGridInterface_;
    GpsTimeIfacePtr    &gpsTimeInterface_;

    // driver for the hardware
    Driver *hwDriver_;

    orca::Frame3d antennaOffset_;

    orcagpsutil::mgaMapgrid mgaMapgrid_;

    orcaice::Context context_;
};

}

#endif