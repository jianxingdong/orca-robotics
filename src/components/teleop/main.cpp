/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/application.h>

#include "component.h"

//
// Build the component into a stand-alone application
// (Since we don't expect to run Teleop as a service we could have
// moved the contents of teleopcomponent.h/cpp into main.cpp)
//
int main(int argc, char * argv[])
{
    teleop::Component component;
    bool installCtrlCHandler = true;
    orcaice::Application app( component, installCtrlCHandler );
    return app.main(argc, argv);
}
