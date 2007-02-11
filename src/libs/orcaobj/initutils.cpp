/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#include <iostream>
#include <sstream>
#include <cmath>
#include <IceUtil/Time.h>

#include <orcaobj/initutils.h>
#include <orcaobj/timeutils.h>
#include <orcaobj/mathdefs.h>

// Random doubles and integers
#define RDOUBLE (rand()*M_PI)
#define RINT    (rand())
#define RCHAR   ((char)rand())

using namespace std;

namespace orcaice
{

void
setInit( orca::Frame2d& obj, double x, double y, double orient )
{
    obj.p.x = x;
    obj.p.y = y;
    obj.o   = orient;
}

void
setInit( orca::Frame3d& obj, double x, double y, double z, double pitch, double roll, double yaw )
{
    obj.p.x = x;
    obj.p.y = y;
    obj.p.z = z;
    obj.o.p = pitch;
    obj.o.r = roll;
    obj.o.y = yaw;
}

void
setInit( orca::Twist2d& obj, double vx, double vy, double w )
{
    obj.v.x = vx;
    obj.v.y = vy;
    obj.w   = w;
}

void
setInit( orca::Twist3d& obj, double vx, double vy, double vz, double wx, double wy, double wz )
{
    obj.v.x = vx;
    obj.v.y = vy;
    obj.v.z = vz;
    obj.w.x = wx;
    obj.w.y = wy;
    obj.w.z = wz;
}

void
setInit( orca::Size2d& obj, double l, double w )
{
    obj.l = l;
    obj.w = w;
}

void
setInit( orca::Size3d& obj, double l, double w, double h )
{
    obj.l = l;
    obj.w = w;
    obj.h = h;
}

void
setInit( orca::CartesianPoint& obj, double x, double y, double z )
{
    obj.x = x;
    obj.y = y;
    obj.z = z;
}

void
setInit( orca::Time& obj )
{
    obj.seconds = 0;
    obj.useconds = 0;
}

void
setInit( orca::Position2dData& obj )
{
    orca::Time t;
    setInit( t );
    obj.timeStamp = t;

    setInit( obj.pose );
    obj.motion.v.x = 0.0;
    obj.motion.v.y = 0.0;
    obj.motion.w = 0.0;
    obj.stalled=false;
}

void 
setInit( orca::Waypoint2d & wp )
{
    setInit(wp.target);
    wp.distanceTolerance = 0.0;
    wp.headingTolerance = 0.0;
    setInit(wp.timeTarget);
    wp.maxApproachSpeed = 0.0;
    wp.maxApproachTurnrate = 0.0;
}

void 
setInit( orca::WifiInterface &w )
{
    w.mode = orca::OperationModeUnknown;  
    w.bitrate = 0;  
    w.accessPoint = "00:00:00:00:00:00";  
    w.interfaceName = "";  
    w.status = 0;  
    w.linkQuality = 0;  
    w.signalLevel = 0;  
    w.noiseLevel = 0;  
    w.numInvalidNwid = 0;  
    w.numInvalidCrypt = 0;  
    w.numInvalidFrag = 0;  
    w.numRetries = 0;  
    w.numInvalidMisc = 0;  
    w.numMissedBeacons = 0;  
    w.throughPut = 0;
    w.linkType = orca::LinkQualityTypeUnknown;
    w.maxLinkQuality = 0;
    w.maxSignalLevel = 0;
    w.maxNoiseLevel = 0;
}

void 
setSane( orca::Time& obj )
{
    obj = toOrcaTime( IceUtil::Time::now() );
}

void 
setSane( orca::TimeOfDay& obj )
{
    obj = toOrcaTimeOfDay( IceUtil::Time::now() );

//     obj.hours   = RINT%24;
//     obj.minutes = RINT%59;
//     obj.seconds = double(RINT%59000)/1000.0;
}

void 
setSane( orca::Date& obj )
{
    obj = toOrcaDate( IceUtil::Time::now() );

//     obj.day   = RINT%31;
//     obj.month = RINT%12;
//     obj.year  = RINT%3000;
}

void 
setSane( orca::CartesianPoint2d& obj )
{
    obj.x = double(RINT%100000)/1000.0 - 50.0;
    obj.y = double(RINT%100000)/1000.0 - 50.0;
}

void 
setSane( orca::CartesianPoint& obj )
{
    obj.x = double(RINT%100000)/1000.0 - 50.0;
    obj.y = double(RINT%100000)/1000.0 - 50.0;
    obj.z = double(RINT%100000)/1000.0 - 50.0;
}

void 
setSane( orca::CartesianVelocity2d& obj )
{
    obj.x = double(RINT%100000)/1000.0 - 50.0;
    obj.y = double(RINT%100000)/1000.0 - 50.0;
}

void 
setSane( orca::CartesianVelocity& obj )
{
    obj.x = double(RINT%100000)/1000.0 - 50.0;
    obj.y = double(RINT%100000)/1000.0 - 50.0;
    obj.z = double(RINT%100000)/1000.0 - 50.0;
}

void 
setSane( orca::AngularVelocity& obj )
{
    obj.x = double(RINT%100000)/1000.0 - 50.0;
    obj.y = double(RINT%100000)/1000.0 - 50.0;
    obj.z = double(RINT%100000)/1000.0 - 50.0;
}

void 
setSane( orca::Frame2d& obj )
{
    setSane( obj.p );
    obj.o   = double(RINT%100000)/1000.0 - 50.0;
}

void 
setSane( orca::Frame3d& obj )
{
    setSane( obj.p );
    obj.o.r = double(RINT%100000)/1000.0 - 50.0;
    obj.o.p = double(RINT%100000)/1000.0 - 50.0;
    obj.o.y = double(RINT%100000)/1000.0 - 50.0;
}

void 
setSane( orca::Twist2d& obj )
{
    setSane( obj.v );
    obj.w   = double(RINT%100000)/1000.0 - 50.0;
}

void 
setSane( orca::Twist3d& obj )
{
    setSane( obj.v );
    setSane( obj.w );
}

void 
setSane( orca::Size2d& obj )
{
    obj.l = double(RINT%100000)/1000.0;
    obj.w = double(RINT%100000)/1000.0;
}

void 
setSane( orca::Size3d& obj )
{
    obj.l = double(RINT%100000)/1000.0;
    obj.w = double(RINT%100000)/1000.0;
    obj.h = double(RINT%100000)/1000.0;
}

void 
setSane( orca::Odometry2dData& obj )
{
    orca::Time t;
    setSane( t );
    obj.timeStamp = t;

    setSane( obj.pose );
    setSane( obj.motion );
}

void 
setSane( orca::Position2dData& obj )
{
    orca::Time t;
    setSane( t );
    obj.timeStamp = t;

    setSane( obj.pose );
    setSane( obj.motion );
    obj.stalled=false;
}

void 
setSane( orca::Position3dData& obj )
{
    orca::Time t;
    setSane( t );
    obj.timeStamp = t;

    setSane( obj.pose );
    setSane( obj.motion );
}

void 
setSane( orca::GpsData& obj )
{
    orca::Time t;
    setSane( t );
    obj.timeStamp = t;

    setSane( obj.utcTime );

    obj.latitude = double(RINT%90000)/1000.0;
    obj.longitude = double(RINT%360000)/1000.0;
    obj.altitude = double(RINT%90000)/1000.0;

    obj.speed = double(RINT%1000000)/1000.0;
    obj.climbRate = double(RINT%1000000)/1000.0;

    obj.satellites = RINT%50;
    obj.positionType = RINT%2;

    obj.geoidalSeparation = double(RINT%10000000)/1000.0;
}


void 
setSane( orca::GpsTimeData& obj )
{
    orca::Time t;
    setSane( t );
    obj.timeStamp = t;

    setSane( obj.utcTime );
    setSane( obj.utcDate );
}


void 
setSane( orca::GpsMapGridData& obj )
{
    orca::Time t;
    setSane( t );
    obj.timeStamp = t;

    setSane( obj.utcTime );

    obj.zone = RINT%500;

    obj.northing = double(RINT%800000)/1000.0;
    obj.easting = double(RINT%800000)/1000.0;
    obj.altitude = double(RINT%90000)/1000.0;

    obj.speed = double(RINT%1000000)/1000.0;
    obj.climbRate = double(RINT%1000000)/1000.0;

    obj.positionType = RINT%2;
}

void 
setSane( orca::PowerData& obj, int count )
{
    orca::Time t;
    setSane( t );
    obj.timeStamp = t;

    // set up data structure for 3 batteries
    orca::BatteryData bd;
    for ( int i=0; i<count; ++i ) 
    {   
        stringstream ss;
        ss << "battery" << i;
        bd.name = ss.str();
        bd.voltage = float(double(RINT%100000)/1000.0);
        bd.percent = float(double(RINT%100000)/1000.0);
        bd.secRemaining = float(double(RINT%1000000)/1000.0);

        obj.batteries.push_back( bd );
    }
}

void 
setSane( orca::Velocity2dCommand& obj )
{
    orca::Time t;
    setSane( t );
    obj.timeStamp = t;

    obj.motion.v.x = double(RINT%100000)/1000.0;
    obj.motion.v.y = double(RINT%100000)/1000.0;
    obj.motion.w = double(RINT%100000)/1000.0;
}

void 
setSane( orca::DriveBicycleData& obj )
{
    obj.speed = double(RINT%100000)/1000.0 - 30.0;
    obj.steerAngle = DEG2RAD( double(RINT%100000)/1000.0 - 50.0 );
}

void 
setSane( const orca::RangeScanner2dDataPtr& obj, int numberOfSamples )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    obj->maxRange = 80.0;
    obj->fieldOfView = M_PI;
    obj->startAngle = -(obj->fieldOfView/2.0);

    for ( int i=0; i<numberOfSamples; ++i ) {
        obj->ranges.push_back( fmod((float)RINT,80) );
    }
}

void 
setSane( const orca::LaserScanner2dDataPtr& obj, int numberOfSamples )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    obj->maxRange = 80.0;
    obj->fieldOfView = M_PI;
    obj->startAngle = -(obj->fieldOfView/2.0);

    for ( int i=0; i<numberOfSamples; ++i ) {
        obj->ranges.push_back( fmod((float)RINT,80) );
        obj->intensities.push_back( char(RINT%8) );
    }
}

void 
setSane( orca::OgMapData& obj, int width, int height )
{
    orca::Time t;
    setSane( t );
    obj.timeStamp = t;

    obj.numCellsX = width;
    obj.numCellsY = height;
    setInit( obj.origin );
    obj.metresPerCellX = 1.0;
    obj.metresPerCellY = 1.0;
    obj.data.resize( width*height, char(88) );
    obj.mapType = orca::OgMapOccupancy;
}

void 
setSane( orca::PixMapData& obj, int width, int height )
{
    orca::Time t;
    setSane( t );
    obj.timeStamp = t;

    obj.numCellsX = width;
    obj.numCellsY = height;
    setInit( obj.origin );
    obj.metresPerCellX = 1.0;
    obj.metresPerCellY = 1.0;
    orca::Pixel p;
    p.r=255; p.g=0; p.b=0;
    obj.rgbPixels.resize( width*height, p );
}


void 
setSane( orca::CameraData& obj, int width, int height )
{
    orca::Time t;
    setSane( t );
    obj.timeStamp = t;

    obj.imageWidth = width;
    obj.imageHeight = height;
    obj.format = orca::ImageFormatModeNfi;
    obj.compression = orca::ImageCompressionNone;
    // assume RGB/BGR mode
    obj.image.resize( obj.imageWidth*obj.imageHeight*3, char(88) );
}

} // namespace
