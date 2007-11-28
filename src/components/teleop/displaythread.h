/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_DISPLAY_THREAD_H
#define ORCA2_TELEOP_DISPLAY_THREAD_H

#include <hydroutil/thread.h>
#include <orcaice/context.h>

#include "inputdriver.h"

namespace teleop
{

class DisplayThread : public hydroutil::Thread, public Display
{
public:

    DisplayThread( const orcaice::Context& context );
    virtual ~DisplayThread();

    virtual void run();

//     DisplayThread* displayHandler() { return displayHandler_; };
    
private:

    // generic interface to input hardware
    InputDriver* driver_;

    InputDriver::Config config_;

//     DisplayThread* displayHandler_;
    
    void init();

    // component current context
    orcaice::Context context_;
};

} // namespace

#endif