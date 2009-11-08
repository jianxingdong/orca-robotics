/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef BICYCLE_HARDWARE_THREAD_H
#define BICYCLE_HARDWARE_THREAD_H

#include <memory>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <hydrointerfaces/bicycle.h>
#include <hydrodll/dynamicload.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <hydrorobotdriverutil/statemachine.h>

namespace bicycle {

//
// @brief The thread that runs the driver
//
// @author Alex Brooks
//
class HwThread : public orcaice::SubsystemThread
{

public: 

    HwThread( const orcaice::Context &context );

    // local interface, used by NetThread

    void setCommand( const hydrointerfaces::Bicycle::Command &command );

    // read-only access to the last received command (through setCommand)
    // returns false if no command was received yet
    bool getLastCommand( hydrointerfaces::Bicycle::Command &command ) const;

    // gives access to the internal data store
    gbxiceutilacfr::Store<hydrointerfaces::Bicycle::Data> &dataStore()
        {
            return dataStore_;
        }

private: 
    // from SubsystemThread
    virtual void work();

    // Keeps trying until success or isStopping()
    void enableDriver();

    // Faults can be detected in either read or write threads: have to be careful.
    hydrorobotdriverutil::StateMachine stateMachine_;

    // Stores the data most recently received from the hardware
    gbxiceutilacfr::Store<hydrointerfaces::Bicycle::Data>    dataStore_;
    // Stores incoming commands
    gbxiceutilacfr::Store<hydrointerfaces::Bicycle::Command> commandStore_;

    bool isMotionEnabled_;

    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    // Generic driver for the hardware
    std::auto_ptr<hydrointerfaces::Bicycle> driver_;

    orcaice::Context context_;
};

} // namespace

#endif
