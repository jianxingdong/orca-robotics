/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_KEYBOARD_AND_JOYSTICK_DRIVER_H
#define ORCA2_KEYBOARD_AND_JOYSTICK_DRIVER_H

#include <orcaiceutil/driver.h>
#include <orcaiceutil/ptrproxy.h>

//#include <orca/position2ddata.h>
#include <orca/velocity2ddata.h>


class InputDriver : public orcaiceutil::Driver
{
public:

    InputDriver( orcaiceutil::PtrProxy* commands );
    virtual ~InputDriver();

    virtual void setup( const Ice::PropertiesPtr & );
    virtual void activate();
    virtual void deactivate();

    virtual void run();

    void setupCommunicator( const Ice::CommunicatorPtr & comm ) { comm_=comm; };

private:

    // component/driver interface
    orcaiceutil::PtrProxy* commandProxy_;

    // internal storage for current command
    orca::Velocity2dDataPtr command_;

    void keyboardHandler();
    void keyboardHelp();

    // define the speed limits for the robot
    // at full joystick depression you'll go this fast
    double maxSpeed_; // m/s
    double maxTurnrate_;  // rad/s

    Ice::CommunicatorPtr comm_;
};

#endif
