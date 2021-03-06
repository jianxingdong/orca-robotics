/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_CATCHUTILS_H
#define HYDROICEUTIL_CATCHUTILS_H

#include <gbxutilacfr/subhealth.h>
#include <gbxutilacfr/tracer.h>

namespace hydroiceutil
{


/*!
Catches the following types results in status fault: {std::exception, std::string, char*, "..."}.

Do not call this function outside of a catch block! In this case, there will be no exception to re-throw
and terminate() will be called, resulting in abort().

Returns problem description.

Example:
@verbatim
try
{
    // do something
}
catch ( ... )
{
    hydroiceutil::catchExceptions( context_.tracer(), "initializing hardware driver" );
}
@endverbatim
*/
std::string catchExceptions( gbxutilacfr::Tracer& tracer, const std::string& activity="" );

/*!
Same as catchExceptions() but also sleeps for specified number of milliseconds before returning.
Use this function to slow execution down in case of a persistent error. Default sleep interval is 1000ms.

Example:
@verbatim
while ( !isStopping() )
{
    try
    {
        // do something
    }
    catch ( ... )
    {
        hydroiceutil::catchExceptionsWithSleep( context_.tracer(), "initializing hardware driver", 2000 );

        // possibly do something else, e.g. re-initialize your hardware driver
    }
}
@endverbatim
*/
std::string catchExceptionsWithSleep( gbxutilacfr::Tracer& tracer,
            const std::string& activity,
            int sleepIntervalMSec=1000 );

/*!
Same as catchExceptions() but also changes subsystem's health to newHealth. All status health types are allowed.
Default new health is gbxutilacfr::SubsystemFault.

Example:
@verbatim
while ( !isStopping() )
{
    try
    {
        // do something
    }
    catch ( ... )
    {
        // will issue a status Warning instead of the default Fault.
        hydroiceutil::catchExceptionsWithSleep( "initializing hardware driver", health(), gbxutilacfr::SubsystemWarning );
    }
}
@endverbatim
*/
std::string catchExceptionsWithStatus( const std::string& activity,
            gbxutilacfr::SubHealth& subHealth,
            gbxutilacfr::SubsystemHealth newHealth=gbxutilacfr::SubsystemFault );

/*!
Same as catchExceptionsWithStatus() but also sleeps for specified number of milliseconds before returning.
Use this function to slow execution down in case of a persistent error.

This function is normally called from inside the catch block of the component's main loop, i.e. inside
SubsystemThread::run() function.

Example:
@verbatim
while ( !isStopping() )
{
    try
    {
        // do your stuff
    }
    catch ( ... )
    {
        hydroiceutil::catchExceptionsWithStatusAndSleep( "initializing hardware driver", health() );
    }
}
@endverbatim
*/
std::string catchExceptionsWithStatusAndSleep( const std::string& activity,
            gbxutilacfr::SubHealth& subHealth,
            gbxutilacfr::SubsystemHealth newHealth=gbxutilacfr::SubsystemFault,
            int sleepIntervalMSec=1000 );

} // namespace


#endif
