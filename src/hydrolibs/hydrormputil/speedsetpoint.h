/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRORMPUTIL_SPEEDSETPOINT_H
#define HYDRORMPUTIL_SPEEDSETPOINT_H

#include <gbxsickacfr/gbxiceutilacfr/timer.h>

namespace hydrormputil {

//
// @brief Represents a speed set-point, for managing acceleration limits
//
// @author Alex Brooks
//
class SpeedSetPoint
{

public: 

    SpeedSetPoint( double maxForwardAcceleration, 
                   double maxReverseAcceleration );

    // This needs to be called every time around the driving loop,
    // So we know the call frequency.
    void evaluateDt();

    // Set the set-point
    void set( double speed );

    // Returns: the speed we should send to the robot right now.
    // Sets setPointAlreadyReached to true if we're already at the set point
    //   (i.e. in the absence of a new set() call, we're
    //     returning the same currentSpeed as last time).
    double currentCmdSpeed( bool &setPointAlreadyReached );
    
private: 

    double dt_;

    double maxForwardAcceleration_;
    double maxReverseAcceleration_;

    double setPoint_;
    double currentCmdSpeed_;

    gbxiceutilacfr::Timer timer_;
};

}

#endif
