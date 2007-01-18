/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef LOCALNAVMANAGER_H
#define LOCALNAVMANAGER_H

#include <orcaice/context.h>
#include <orcaice/ptrbuffer.h>
#include <orca/localise2d.h>
#include <orca/velocitycontrol2d.h>
#include <orca/rangescanner2d.h>
#include <orca/pathfollower2d.h>
#include <orcanavutil/orcanavutil.h>
#include <localnavutil/idriver.h>
#include <orcaice/heartbeater.h>

namespace localnav {

class LocalNavParameters;
class Goal;
class PathMaintainer;

//
// Takes goals and localisation info in the global reference frame.
// Feeds information to the driver, all in the robot's local frame
// of reference.
//
// @author Alex Brooks
//
class LocalNavManager
{

public: 

    LocalNavManager( IDriver   &driver,
                     PathMaintainer   &pathMaintainer,
                     const orcaice::Context & context );

    // The odometry is required for the velocity, which isn't contained
    // in Localise2d.
    void getCommand( const orca::RangeScanner2dDataPtr  rangeData,
                     const orca::Localise2dData&        localiseData,
                     const orca::Odometry2dData&        odomData,
                     orca::VelocityControl2dData&       cmd );

private: 

    void maybeSendHeartbeat();
    
    // Returns true iff a waypoint is active
    bool getCurrentGoal( const orcanavutil::Pose &pose,
                         Goal &currentGoal );

    // The driver itself
    IDriver   &driver_;

    // This thing is responsible for keeping track of where we are in the path.
    // It's updated asynchronously by the outside world.
    PathMaintainer   &pathMaintainer_;

    orca::Waypoint2d  currentWaypoint_;

    // Maintain these for heartbeat messages
    double            secondsBehindSchedule_;

    orcaice::Heartbeater heartbeater_;
    orcaice::Context  context_;
};

}

#endif
