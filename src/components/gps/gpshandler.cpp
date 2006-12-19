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
 
#include <iostream>
#include <orcaice/orcaice.h>

#include "gpshandler.h"

using namespace std;
using namespace orca;
using namespace orcaice;

GpsHandler::GpsHandler(GpsI              &gpsObj,
                       GpsDriver         *hwDriver,
                       orcaice::Context  context,
                       bool              startEnabled )
    : gpsObj_(gpsObj),
      hwDriver_(hwDriver),
      context_(context)
{
}

GpsHandler::~GpsHandler()
{
}

void
GpsHandler::run()
{
    const int TIME_BETWEEN_HEARTBEATS  = 10000;  // ms
    IceUtil::Time lastHeartbeatTime = IceUtil::Time::now();

    try 
    {
	GpsDescriptionPtr descr_ = gpsObj_.localGetDescription();
        GpsDataPtr gpsData = new GpsData;
        GpsMapGridDataPtr gpsMapGridData = new GpsMapGridData;
        GpsTimeDataPtr gpsTimeData = new GpsTimeData;

        //
        // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
        //            So we have to avoid getting stuck in a loop anywhere within this main loop.
        //
        while ( isActive() )
        {
            //
            // This 'if' block is what slows the loop down, by either reading from the GPS
            // or sleeping.
            //
            if ( hwDriver_->isEnabled() )
	    {
                // Read from the GPS
                int ret = hwDriver_->read();
                if ( ret == -1 ){
                    context_.tracer()->error( "Problem reading from GPS.  Shutting down hardware." );
                    hwDriver_->disable();
		}

		if( hwDriver_->hasFix() ){
		    // GPS data tends to come in bursts in 1 second intervals
		    // hwDriver_->read() will return after emptying serial buffer
		    // this should provide a nice 1s loop frequency
                    // may need to be adjusted for fancier GPSes

		    if(hwDriver_->getData(gpsData)==0){
                        int zone;
			zone=mgaMapgrid_.getGridCoords(gpsData->latitude, gpsData->longitude,
						      gpsMapGridData->easting,gpsMapGridData->northing);
			gpsMapGridData->zone=zone;
                        //copy across all the other stuff
                        gpsMapGridData->utcTime=gpsData->utcTime;
			gpsMapGridData->altitude=gpsData->altitude;

			gpsMapGridData->heading=gpsData->heading;
			gpsMapGridData->speed=gpsData->speed;
			gpsMapGridData->climbRate=gpsData->climbRate;
			gpsMapGridData->positionType=gpsData->positionType;

			gpsObj_.localSetData(gpsData);

			//correct for local frame
			CartesianPoint p;
                        //copy out point
			p.x=gpsMapGridData->easting;
			p.y=gpsMapGridData->northing;
			p.z=gpsMapGridData->altitude;
                        //convert
			p=convertToFrame3d(descr_->offset,p);
                        // reset object
			gpsMapGridData->easting=p.x;
			gpsMapGridData->northing=p.y;
			gpsMapGridData->altitude=p.z;

                        gpsObj_.localSetMapGridData(gpsMapGridData);

		    }

		    if(hwDriver_->getTimeData(gpsTimeData)==0){
                      gpsObj_.localSetTimeData(gpsTimeData);
		    }

		}
            }
            else
            {
                // Wait for someone to enable us
                IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));
            }

            if ( (IceUtil::Time::now()-lastHeartbeatTime).toMilliSecondsDouble() >= TIME_BETWEEN_HEARTBEATS )
            {
                if ( hwDriver_->isEnabled() )
                {
                    context_.status()->heartbeat("Gps enabled. " + hwDriver_->heartbeatMessage() );
                }
                else
                {
                    context_.status()->heartbeat( "Gps disabled." );
                }
                lastHeartbeatTime = IceUtil::Time::now();
            }
        } // end of while
    } // end of try
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    
    // GPS hardware will be shut down in the driver's destructor.
    context_.tracer()->debug( "dropping out from run()", 5 );
}
