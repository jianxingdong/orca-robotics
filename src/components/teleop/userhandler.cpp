/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
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

#include <orcaice/orcaice.h>

#include "userhandler.h"

#ifdef HAVE_KEYBOARD_DRIVER
    #include "keyboard/teleopkeyboarddriver.h"
#endif
#ifdef HAVE_JOYSTICK_DRIVER
    #include "joystick/teleopjoystickdriver.h"
#endif
#include "teleopfakedriver.h"


using namespace std;
using namespace orca;

UserHandler::UserHandler( orcaice::PtrBuffer<orca::Velocity2dCommandPtr> *commands,
                    const orcaice::Context & context )
      : commandPipe_(commands),
        driver_(0),
        context_(context)
{
    // this is the last place we can throw exceptions from.
    // after this the thread will be launched
    init();
}

UserHandler::~UserHandler()
{
    delete driver_;
}

void UserHandler::init()
{
    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";
    
    config_.maxSpeed = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
                prefix+"MaxSpeed", 1.0 );
    config_.maxTurnrate = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
                prefix+"MaxTurnrate", 40.0 )*DEG2RAD_RATIO;

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( context_.properties(),
                prefix+"Driver", "keyboard" );
    if ( driverName == "keyboard" )
    {
#ifdef HAVE_KEYBOARD_DRIVER
        context_.tracer()->print("loading keyboard driver");
        driver_ = new TeleopKeyboardDriver( config_ );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'keyboard' because it was not built!" );
#endif
    }
    else if ( driverName == "joystick" )
    {
#ifdef HAVE_JOYSTICK_DRIVER
        context_.tracer()->print("loading joystick driver");
        
        config_.joystickDevice = orcaice::getPropertyWithDefault( context_.properties(),
                "Teleop.Config.JoystickDevice", "/dev/input/event0" );
        driver_ = new TeleopJoystickDriver( config_ );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'joystick' because it was not built!" );
#endif
    }
    else if ( driverName == "fake" )
    {
        context_.tracer()->print("loading fake driver");
        driver_ = new TeleopFakeDriver( config_ );
    }
    else {
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        context_.tracer()->error( errorStr);
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }
}

// read commands from the keyboard. Launced in a separate thread.
void UserHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    // don't forget to enable the driver, but check isActive() to see if we should quit
    while ( driver_->enable() && isActive() ) {
        context_.tracer()->warning("failed to enable driver");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    }

    // check again to make sure we are not being terminated
    if ( !isActive() ) {
        return;
    }
    
    context_.tracer()->print("driver enabled");
    
    Velocity2dCommandPtr currCommand = new Velocity2dCommand;
    Velocity2dCommandPtr lastCommand = new Velocity2dCommand;
    
    while ( isActive() )
    {
        // remember last command so we can tell if anything has changed
        lastCommand->motion.v.x = currCommand->motion.v.x;
        lastCommand->motion.w = currCommand->motion.w;

        // Read from the input
        driver_->readdata( currCommand );

        // commit change only if something has actually changed
        if ( lastCommand->motion.v.x != currCommand->motion.v.x ||
             lastCommand->motion.w != currCommand->motion.w )
        {
            commandPipe_->push( currCommand );
        }
    }

    // reset the hardware
    if ( driver_->disable() ) {
        context_.tracer()->warning("failed to disable driver");
    }
    context_.tracer()->debug("driver disabled");

    } // try
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        context_.communicator()->destroy();
    }
}
