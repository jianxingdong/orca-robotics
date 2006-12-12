/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <cmath>
// M_PI is not defined after including cmath for the MS visual studio compiler?
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <orcaobj/timeutils.h>
#include <orcaobj/miscutils.h>

#include "stringutils.h"

// this is a copy from orcaice/mathdefs.h
// make a copy here to cut circular dependency
//==============
#ifndef DEG2RAD_RATIO
#define DEG2RAD_RATIO	(M_PI/180.0)
#endif

//! Converts from degrees to radians.
#define DEG2RAD(deg)	((deg)*DEG2RAD_RATIO)
//! Converts from radians to degrees.
#define RAD2DEG(rad) 	((rad)/DEG2RAD_RATIO)
//==============

using namespace std;

namespace orcaice
{

std::string 
toString( const orca::FQComponentName & name )
{
    return name.platform + "/" + name.component;
}

std::string 
toString( const orca::FQInterfaceName & name )
{
    return name.iface + "@" + name.platform + "/" + name.component;
}

std::string 
toString( const orca::FQTopicName & name )
{
    return name.iface + "/" + name.topic + "@" + name.platform + "/" + name.component;
}

std::string toString( const orca::FQExecutableName & name )
{
    return name.executable + "@" + name.host ;
}

orca::FQComponentName 
toComponentName( const std::string & s )
{
    orca::FQComponentName fqCName;

    int posSlash = s.find( '/', 0 );
    //cout<<"/ found :"<<posSlash<<endl;

    if ( posSlash<0 ) { 
        // delimeter not found: return null structure
    }
    else {
        fqCName.platform = s.substr( 0, posSlash );
        fqCName.component = s.substr( posSlash+1, s.npos );
    }

    //cout<<"debug: "<<toString(fqCName)<<endl;
    return fqCName;
}

orca::FQInterfaceName 
toInterfaceName( const std::string & s )
{
    orca::FQInterfaceName fqIName;

    int posAt = s.find( '@', 0 );
    //cout<<"@ found :"<<posAt<<endl;
    int posSlash = s.find( '/', 0 );
    //cout<<"/ found :"<<posSlash<<endl;

    if ( posAt<0 || posSlash<0 ) { 
        // delimeters not found: return null structure
    }
    else {
        fqIName.iface = s.substr( 0, posAt );
        fqIName.platform = s.substr( posAt+1, posSlash-posAt-1 );
        fqIName.component = s.substr( posSlash+1, s.npos );
    }

    //cout<<"debug: "<<toString(fqIName)<<endl;
    return fqIName;
}

orca::FQTopicName 
toStatusTopic( const orca::FQComponentName & fqCName )
{
    orca::FQTopicName fqTName;
        
    fqTName.platform = fqCName.platform;
    fqTName.component = fqCName.component;
    fqTName.iface = "status";
    fqTName.topic = "*";

    return fqTName;
}


//**************************
// toString()
//*************************

std::string 
toString( const orca::BinarySwitchDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
        << " BinarySwitch ["<<obj->devices.size()<<" devices] (name,state) :";

    for ( unsigned int i=0; i < obj->devices.size(); i++ )
    {
        s << endl << "    " << i << " [" 
          <<obj->devices[i].name<<","
          <<(int)obj->devices[i].state<<"]";
    }
    return s.str();
}

std::string 
toString( const orca::CameraDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
        << " CameraData: ["<<obj->image.size()<<" bytes]\n" 
        << "Image height              " << obj->imageHeight << "pix\n"
        << "Image width               " << obj->imageWidth << "pix\n"
        << "Format                    " << obj->format << "\n"
        << "Compression               " << obj->compression << "\n";
    return s.str();
}

std::string 
toString( const orca::CameraDescriptionPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
        << " CameraDescription:\n" 
        << "Image height              " << obj->imageHeight << "pix\n"
        << "Image width               " << obj->imageWidth << "pix\n"
        << "Frame rate                " << obj->frameRate << "fps\n"
        << "Format                    " << obj->format << "\n"
        << "Compression               " << obj->compression << "\n"
        << "offset.point.x            " << obj->offset.p.x << "m\n"
        << "offset.point.y            " << obj->offset.p.y << "m\n"
        << "offset.point.z            " << obj->offset.p.z << "m\n"
        << "offset.orientation.roll   " << RAD2DEG(obj->offset.o.r) << "deg\n"
        << "offset.orientation.pitch  " << RAD2DEG(obj->offset.o.p) << "deg\n"
        << "offset.orientation.yaw    " << RAD2DEG(obj->offset.o.y) << "deg\n";
    return s.str();
}

std::string 
toString( const orca::CpuInfoPtr & obj )
{
    std::ostringstream s;
    //s << toString(obj->timeStamp)
    s  << " CpuInfo: info = " << obj->info <<endl;

    return s.str();
}

std::string 
toString( const orca::CpuDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp);
    s  << " CpuData: user = " << obj->userLoad <<"%";
    s << " sys = " << obj->systemLoad <<"%";
    s << " idle = " << obj->idleLoad <<"%";
    s << " temp = " << obj->temperature <<"C"<<endl;

    return s.str();
}

std::string 
toString( const orca::GpsDescriptionPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
        << " GpsDescription: \n"
        << "size.l                " << obj->size.l << "m\n"
        << "size.w                " << obj->size.w << "m\n"
        << "size.h                " << obj->size.h << "m\n"
        << "offset.point.x            " << obj->offset.p.x << "m\n"
        << "offset.point.y            " << obj->offset.p.y << "m\n"
        << "offset.point.z            " << obj->offset.p.z << "m\n"
        << "offset.orientation.roll   " << RAD2DEG(obj->offset.o.r) << "deg\n"
        << "offset.orientation.pitch  " << RAD2DEG(obj->offset.o.p) << "deg\n"
        << "offset.orientation.yaw    " << RAD2DEG(obj->offset.o.y) << "deg\n";
    return s.str();
}

std::string 
toString( const orca::GpsDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp) 
        << " UTC: " << toString(obj->utcTime) << endl
	<< " Gps (latitude,longitude,altitude) : ("
        << obj->latitude << ","
	<< obj->longitude << ","
	<< obj->altitude << ")" << endl
        << " Gps (heading,speed,climbrate) : ("
        << obj->heading << ","
	<< obj->speed << ","
	<< obj->climbRate << ")" << endl
        << " Gps (satellites,positionType,geoidalSeparation) : ("
	<< obj->satellites << ","
	<< obj->positionType << ","
        << obj->geoidalSeparation << ")";

    return s.str();
}

std::string 
toString( const orca::GpsMapGridDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp) 
        << " UTC: " << toString(obj->utcTime) << endl
	<< " Gps (northing,easting,altitude) : ("
        << obj->northing << ","
	<< obj->easting << ","
	<< obj->altitude << ")" << endl
        << " Gps (heading,speed,climbrate) : ("
        << obj->heading << ","
	<< obj->speed << ","
	<< obj->climbRate << ")" << endl
	<< " Gps (zone, positionType) : ("
        << obj->zone << ","
	<< obj->positionType << ")";

    return s.str();
}

std::string 
toString( const orca::GpsTimeDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp) 
      << " UTC: " << toString(obj->utcDate) << " " << toString(obj->utcTime);
    return s.str();
}

std::string 
toString( const orca::Time & obj )
{
    std::ostringstream s;
    s << toIceTime( obj ).toString();
    // old style. should we provide both somehow?
//     s << "Time (s,us) : ("
//             << obj.seconds << ","
//             << obj.useconds << ")";

    return s.str();
}

std::string 
toString( const orca::TimeOfDay & obj )
{
    char c[64];
    sprintf(c,"%02d:%02d:%06.3f", obj.hours, obj.minutes, obj.seconds);
    return std::string(c);
}

std::string 
toString( const orca::Date & obj )
{
    char c[64];
    sprintf(c,"%04d/%02d/%02d", obj.year, obj.month, obj.day);
    return std::string(c);
}

std::string 
toString( const orca::Position2dDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
        << " Position2d (x,y,a(deg), vx,vy,w(deg/s)) : ("
        << obj->pose.p.x << ","
        << obj->pose.p.y << ","
        << RAD2DEG(obj->pose.o) << ", "
        << obj->motion.v.x << ","
        << obj->motion.v.y << ","
        << RAD2DEG(obj->motion.w) << ")";

    return s.str();
}

std::string 
toString( const orca::Position3dDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
            << " \t Position (x,y,z): "
            << obj->pose.p.x << ","
            << obj->pose.p.y << ","
            << obj->pose.p.z << ", \n"
            << " \t Attitude r,p,y(deg): "     
            << RAD2DEG(obj->pose.o.r) << ", "
            << RAD2DEG(obj->pose.o.p) << ", "
            << RAD2DEG(obj->pose.o.y) << ", \n"
            << " \t Velocity vx,vy,vz:"     
            << obj->motion.v.x << ","
            << obj->motion.v.y << ","
            << obj->motion.v.z << ", \n"
            << " \t Angular Velocity wx,wy,wz(deg/s): "     
            << RAD2DEG(obj->motion.w.x) << ","
            << RAD2DEG(obj->motion.w.y) << ","
            << RAD2DEG(obj->motion.w.z);

    return s.str();
}

std::string 
toString( const orca::Position3dDescriptionPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
            << " \t Position (x,y,z): "
            << obj->offset.p.x << ","
            << obj->offset.p.y << ","
            << obj->offset.p.z << ", \n"
            << " \t Attitude r,p,y(deg): "     
            << RAD2DEG(obj->offset.o.r) << ", "
            << RAD2DEG(obj->offset.o.p) << ", "
            << RAD2DEG(obj->offset.o.y) << ", \n"
            << " \t Size l,w,h: "
            << obj->size.l << ", "
            << obj->size.w << ", "
            << obj->size.h;
    
    return s.str();   
}

std::string 
toString( const orca::Localise2dDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
        << " Localise2d ["<<obj->hypotheses.size()<<" elements] [weight] (x,y,a(deg) (pxx,pxy,pyy,pxt,pyt,pyy(deg)) :"<<endl;
    for ( unsigned int i=0; i < obj->hypotheses.size(); i++ )
    {
        const orca::Pose2dHypothesis &h = obj->hypotheses[i];
        s << "  " << i << ": [" 
          << h.weight << "] (" 
          << h.mean.p.x << ","
          << h.mean.p.y << ","
          << h.mean.o*180.0/M_PI << "deg) ("
          << h.cov.xx << ","
          << h.cov.xy << ","
          << h.cov.yy << ","
          << h.cov.xt << ","
          << h.cov.yt << ","
          << h.cov.tt*180.0/M_PI << "deg)" << endl;
    }
    return s.str();
}

std::string 
toString( const orca::Velocity2dCommandPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
        << " Velocity2dCommand (vx,vy,w(deg/s)) : ("
            << obj->motion.v.x << ", "
            << obj->motion.v.y << ", "
            << RAD2DEG(obj->motion.w) << ")";

    return s.str();
}

std::string 
toString( const orca::HomeDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp) << " HomeData"<<endl;

    IceUtil::Time t = IceUtil::Time::seconds(obj->timeUp);
    s << "time up="<<t.toDuration()<<endl;

    s << "provides={";
    for ( unsigned int i=0; i<obj->comp.provides.size(); ++i ) {
        if ( i>0) { s<<":"; }
        s << obj->comp.provides[i].name;
    }
    s << "}"<<endl;

    s << "requires={";
    for ( unsigned int i=0; i<obj->comp.requires.size(); ++i ) {
        if ( i>0) { s<<":"; }
        s << toString( obj->comp.requires[i].name );
    }
    s << "}" << endl;

    return s.str();
}

std::string 
toString( const orca::ImuDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
        << " ImuData: \n"
        << " Accelerometer (x,y,z): "
        << obj->accel.x << " "
        << obj->accel.y << " "
        << obj->accel.z << " " << "\n"
        << " Gyro (x,y,z): "
        << obj->gyro.x << " "
        << obj->gyro.y << " "
        << obj->gyro.z << " ";
    
    return s.str();
}

std::string 
toString( const orca::LaserScanner2dDataPtr & obj, int skip )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
      << " LaserData [" << obj->ranges.size() << " elements]";

    s << "minRange=" << obj->minRange << "m" << endl;
    s << "maxRange=" << obj->maxRange << "m" << endl;
    s << "fieldOfView=" << obj->fieldOfView * 180.0/M_PI << "deg" << endl;
    s << "startAngle=" << obj->startAngle * 180.0/M_PI << "deg" << endl;

    if ( skip > -1 ) 
    {
        s << ": (";
        for ( unsigned int i=0; i < obj->ranges.size(); ++i ) {
            s << obj->ranges[i] << ":" << (int) obj->intensities[i] << " ";
            i = i + skip;
        }
        s << ")";
        s << endl;
    }
    return s.str();
}

std::string 
toString( const orca::RangeScanner2dDataPtr & obj, int skip )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
      << " RangeScanner2dData [" << obj->ranges.size() << " elements]: " << endl;

    s << "minRange=" << obj->minRange << "m" << endl;
    s << "maxRange=" << obj->maxRange << "m" << endl;
    s << "fieldOfView=" << obj->fieldOfView * 180.0/M_PI << "deg" << endl;
    s << "startAngle=" << obj->startAngle * 180.0/M_PI << "deg" << endl;

    if ( skip > -1 ) 
    {
        s << ": (";
        for ( unsigned int i=0; i < obj->ranges.size(); ++i ) {
            s << obj->ranges[i] << " ";
            i = i + skip;
        }
        s << ")";
        s << endl;
    }
    return s.str();
}

std::string 
toString( const orca::PowerDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
        << " Power ["<<obj->batteries.size()<<" batteries] (name,volt,%,sec) :";

    for ( unsigned int i=0; i < obj->batteries.size(); ++i )
    {
        s << endl << "    [" 
          <<obj->batteries[i].name<<","
          <<obj->batteries[i].voltage<<"V,"
          <<obj->batteries[i].percent<<"%,"
          <<obj->batteries[i].secRemaining<<"s]";
    }
    return s.str();
}

std::string 
toString( const orca::StatusDataPtr & obj )
{
    std::ostringstream s;
    // this toString() function is defined in nameutils.h
    s << toString( obj->name ) << ": "
      << obj->category << ": "
      << obj->message;

    return s.str();
}

std::string 
toString( const orca::RangeScanner2dDescriptionPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
        << " RangeScanner2dDescription: \n"
        << "minRange=" << obj->minRange << "m\n"
        << "maxRange=" << obj->maxRange << "m\n"
        << "fieldOfView=" << RAD2DEG(obj->fieldOfView) << "deg\n"
        << "startAngle=" << RAD2DEG(obj->startAngle) << "deg\n"
        << "numberOfSamples=" << obj->numberOfSamples << "\n"
        << "offset.point.x=" << obj->offset.p.x << "m\n"
        << "offset.point.y=" << obj->offset.p.y << "m\n"
        << "offset.point.z=" << obj->offset.p.z << "m\n"
        << "offset.orientation.roll=" << RAD2DEG(obj->offset.o.r) << "deg\n"
        << "offset.orientation.pitch=" << RAD2DEG(obj->offset.o.p) << "deg\n"
        << "offset.orientation.yaw=" << RAD2DEG(obj->offset.o.y) << "deg\n"
        << "size.length=" << obj->size.l << "m\n"
        << "size.width=" << obj->size.w << "m\n"
        << "size.height=" << obj->size.h << "m";

    return s.str();
}

std::string 
toString( const orca::ImuDescriptionPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
            << " ImuDescription: \n"
            << "Size: \n"
            << "size.l:           " << obj->size.l << "m\n"
            << "size.w:           " << obj->size.w << "m\n"
            << "size.h:           " << obj->size.h << "m\n"
            << "Offset: \n"
            << "origin.p.x:           " << obj->offset.p.x << "m\n"
            << "offset.p.y:           " << obj->offset.p.y << "m\n"
            << "offset.p.z:           " << obj->offset.p.z << "m\n"
            << "offset.o.r:           " << obj->offset.o.r << "m\n"
            << "offset.o.p:           " << obj->offset.o.p << "m\n"
            << "offset.o.y:           " << obj->offset.o.y << "m\n";

    return s.str();
}

std::string 
toString( const orca::PolarFeature2dDataPtr & obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
      << " PolarFeatureData [" << obj->features.size() << " elements] (type,pFalsePos,pTruePos) (details): " << endl;

    const orca::PolarFeature2dSequence &features = obj->features;
    for (unsigned int i=0; i < obj->features.size(); i++)
    {
        if ( features[i] == 0 )
        {
            s << "  " << i << ": ERROR: Feature was NULL!" << endl;
            continue;
        }

        const orca::SinglePolarFeature2dPtr &ftr = features[i];
        s << "  " << i << ": "
          << "(t" << ftr->type
          << ",pf" << ftr->pFalsePositive
          << ",pt" << ftr->pTruePositive
          << ")";

        // a bit ugly...
        if ( ftr->ice_isA( "::orca::PointPolarFeature2d" ) )
        {
            const orca::PointPolarFeature2d& f = dynamic_cast<const orca::PointPolarFeature2d&>(*ftr);
            s << "(r="<<f.p.r<<",b="<<f.p.o*180.0/M_PI << "deg)"<<endl;
        }
        else if ( ftr->ice_isA( "::orca::LinePolarFeature2d" ) )
        {
            const orca::LinePolarFeature2d& f = dynamic_cast<const orca::LinePolarFeature2d&>(*ftr);
            s << "(r="<<f.start.r<<",b="<<f.start.o*180.0/M_PI << "deg -> r="
              <<f.end.r<<",b="<<f.end.o*180.0/M_PI << "deg)" << endl;
        }
        else
        {
            s << "(generic)" << endl;
        }
    }
    s << endl;
    
    return s.str();
}

std::string 
toString( const orca::Frame2d & obj )
{
    std::ostringstream s;
    s << "[" << obj.p.x << "m," << obj.p.y << "m," << obj.o*180.0/M_PI << "deg]";
    return s.str();
}

std::string 
toString( const orca::Frame3d & obj )
{
    std::ostringstream s;
    s << "[" <<obj.p.x<<"m," <<obj.p.y<<"m," <<obj.p.z<<"m, " 
      << obj.o.r*180.0/M_PI<<"," <<obj.o.p*180.0/M_PI<<"," <<obj.o.y*180.0/M_PI << "deg]";
    return s.str();
}

std::string 
toString( const orca::OgMapDataPtr &obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
      << " OgMap: \n"
      << "origin:        [" << obj->origin.p.x << ", " << obj->origin.p.y << ", " << obj->origin.o*180.0/M_PI << "]\n"
      << "numCells:      [" << obj->numCellsX << ", " << obj->numCellsY << "]\n"
      << "metresPerCell: [" << obj->metresPerCellX << ", " << obj->metresPerCellY << "]\n";

    return s.str();
}

std::string 
toString( const orca::PathFollower2dDataPtr &obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
      << " PathFollower2dData: " << obj->path.size() << " waypoints.";
    return s.str();
}

std::string 
toString( const orca::Waypoint2d &obj )
{
    std::ostringstream s;
    s << "target=" << toString(obj.target) 
      << ", tolerances=[" << obj.distanceTolerance << "," << obj.headingTolerance*180.0/M_PI << "deg]"
      << ", timeTarget=[" << toString(obj.timeTarget) << "]"
      << ", maxApproach=[" << obj.maxApproachSpeed << "m/s," << obj.maxApproachTurnrate*180.0/M_PI << "deg/s]";
    return s.str();
}

std::string 
toString( const orca::FeatureMap2dDataPtr &obj )
{
    std::ostringstream s;
    assert( obj != 0 );

    s << toString(obj->timeStamp)
      << " FeatureMap2d ["<<obj->features.size()<<" elements]: "<<endl;

    for (unsigned int i=0; i < obj->features.size(); i++)
    {
        assert( obj->features[i] != 0 );

        //
        // I'm not convinced that this is the best way of doing things...
        //
        const orca::Feature2dPtr &f = obj->features[i];
        s << "  " << i << ": [t"<<f->type<<",p"<<f->pExists<<"] ";

        if ( f->ice_isA( "::orca::CartesianPointFeature2d" ) )
        {
            const orca::CartesianPointFeature2d& r = dynamic_cast<const orca::CartesianPointFeature2d&>(*f);
            s << "("<<r.p.x<<","<<r.p.y<<") ("<<r.c.xx<<","<<r.c.xy<<","<<r.c.yy<<")";
        }
        else if ( f->ice_isA( "::orca::CartesianLineFeature2d" ) )
        {
            const orca::CartesianLineFeature2d& r = dynamic_cast<const orca::CartesianLineFeature2d&>(*f);
            s << "(r="<<r.rho<<",a="<<r.alpha*180.0/M_PI<<"deg) ("<<r.c.xx<<","<<r.c.xy<<","<<r.c.yy<<")" << " ("<<r.start.x<<","<<r.start.y<<")->("<<r.end.x<<","<<r.end.y<<")";
        }
        else
        {
            // Don't really know how to display info about this feature.
            assert( false );
        }
        s << endl;
    }    
    s << endl;
    
    return s.str();
}

//**************************
// toVerboseString()
//*************************

std::string 
toVerboseString( const orca::OgMapDataPtr &map )
{
    std::stringstream ss;
    int szX=map->numCellsX;
    int szY=map->numCellsY;
    int x, y;
    int step = 1 + map->numCellsX / 200;

    ss<<endl;
    ss<<"      ";
    for( x=0; x<szX; x+=step )
    {
        ss<<"-";
    }
    ss<<"-"<<endl;

    for( y=szY-1; y>=0; y-=step)
    {
        ss<<"     | ";
        for( x=0; x<szX; x+=step)
        {
            ss<<displayOgmapCell( gridCell(map,x,y) );
        }
        ss<<"|"<<endl;
    }

    ss<<"  y   ";
    for( x=0; x<szX; x+=step )
    {
        ss<<"-";
    }
    ss<<"-"<<endl;

    ss<<"  |"<<endl;
    ss<<"  |____ x"<<endl;
    return ss.str();
}

std::string 
toVerboseString( const orca::PathFollower2dDataPtr &obj )
{
    std::stringstream s;

    s << orcaice::toString(obj->timeStamp)
      << " PathFollower2dData [" << obj->path.size() << " waypoints]:" << endl;
    for ( unsigned int i=0; i < obj->path.size(); i++ )
    {
        s << "  " << i << ":" << orcaice::toString(obj->path[i]) << endl;
    }

    return s.str();
}

std::string 
toVerboseString( const orca::PathPlanner2dDataPtr &obj )
{
    std::stringstream s;

    s << orcaice::toString(obj->timeStamp)
      << " PathPlanner2dDataPtr [" << obj->path.size() << " waypoints. Result code: " << obj->result << " ]:" << endl;
    for ( unsigned int i=0; i < obj->path.size(); i++ )
    {
        s << "  " << i << ":" << orcaice::toString(obj->path[i]) << endl;
    }

    return s.str();
}

std::string 
toVerboseString( const orca::PathPlanner2dTaskPtr &obj )
{
    std::stringstream s;

    s << orcaice::toString(obj->timeStamp)
      << " PathPlanner2dTaskPtr [path contains "<< obj->coarsePath.size() << " waypoints]:" << endl;
    for ( unsigned int i=0; i < obj->coarsePath.size(); i++ )
    {
        s << "  " << i << ":" << orcaice::toString(obj->coarsePath[i]) << endl;
    }

    return s.str();
}

std::string 
toVerboseString( const orca::ImuDataPtr &obj )
{
    std::stringstream s;

    s << orcaice::toString(obj->timeStamp)
        << " ImuData: \n"
        << "Accelerometer (x,y,z): "
        << obj->accel.x << " "
        << obj->accel.y << " "
        << obj->accel.z << " \n"
        << "Gyro (x,y,z): "
        << obj->gyro.x << " "
        << obj->gyro.y << " "
        << obj->gyro.z << " ";

    return s.str();
}

std::string 
toVerboseString( const orca::Position3dDataPtr &obj )
{
    std::stringstream s;
    s << toString(obj->timeStamp)
            << " \t Position (x,y,z): "
            << obj->pose.p.x << ","
            << obj->pose.p.y << ","
            << obj->pose.p.z << ", \n"
            << " \t Attitude r,p,y(deg): "     
            << RAD2DEG(obj->pose.o.r) << ", "
            << RAD2DEG(obj->pose.o.p) << ", "
            << RAD2DEG(obj->pose.o.y) << ", \n"
            << " \t Velocity vx,vy,vz:"     
            << obj->motion.v.x << ","
            << obj->motion.v.y << ","
            << obj->motion.v.z << ", \n"
            << " \t Angular Velocity wx,wy,wz(deg/s): "     
            << RAD2DEG(obj->motion.w.x) << ","
            << RAD2DEG(obj->motion.w.y) << ","
            << RAD2DEG(obj->motion.w.z);

    return s.str();
}

} // namespace
