/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft, Michael Warren
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCALOGFACTORY_STRINGIFY_UTILITIES_H
#define ORCALOGFACTORY_STRINGIFY_UTILITIES_H

#include <string>
#include <orca/common.h>
#include <orca/camera.h>
#include <orca/cpu.h>
#include <orca/drivebicycle.h>
#include <orca/gps.h>
#include <orca/ogmap.h>
#include <orca/home.h>
#include <orca/imu.h>
#include <orca/laserscanner2d.h>
#include <orca/localise2d.h>
#include <orca/localise3d.h>
#include <orca/power.h>
#include <orca/status.h>
#include <orca/polarfeature2d.h>
#include <orca/featuremap2d.h>
#include <orca/pathplanner2d.h>
#include <orca/wifi.h>
#include <orca/multicamera.h>

namespace orcalogfactory
{
/*!
 *  @name Stringify Orca structures for logging.
 */
//@{

//! Prints out cpu data to text which is easy to parse
std::string toLogString( const orca::CpuData& );

//! Prints out gps data to text which is easy to parse
std::string toLogString( const orca::GpsData& );
//! Parses from ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::GpsData& );

std::string toLogString( const orca::RangeScanner2dDescription& );
void fromLogString( std::stringstream &, orca::RangeScanner2dDescription& );
std::string toLogString( const orca::LaserScanner2dDataPtr& );
void fromLogString( std::stringstream &, orca::LaserScanner2dData& );
std::string toLogString( const orca::Localise2dData& );
void fromLogString( std::stringstream &, orca::Localise2dData & );
std::string toLogString( const orca::Localise3dData& );
//! Prints out resulting fine path to text in a parsable format
std::string toLogString( const orca::PathPlanner2dData& );
//! Prints out task path (coarse) to text in a parsable format
std::string toLogString( const orca::PathPlanner2dTask& );
//! Prints out polarfeature2d data to text which is easy to parse
std::string toLogString( const orca::PolarFeature2dData& );        
//! Prints out imu data to text which is easy to parse
std::string toLogString( const orca::ImuData& obj );
//! Prints out wifi data to text which is easy to parse
std::string toLogString( const orca::WifiData& obj );
std::string toLogString( const orca::DriveBicycleData& );

//! Prints out Power data to text which is easy to parse
std::string toLogString( const orca::PowerData& );
//! Parses a line of ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::PowerData& );

//! Returns string in simple format "sec usec" 
std::string toLogString( const orca::Time& );
//! Parses a line of ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::Time& );

//! Prints out gpsdescription to text which is easy to parse
std::string toLogString( const orca::GpsDescription& );
//! Parses from ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::GpsDescription& );

//! Converts vehicle geometry to text which is easy to parse
std::string toLogString( const orca::VehicleGeometryDescriptionPtr& );
void fromLogString( std::stringstream &, orca::VehicleGeometryDescriptionPtr& );
//! Converts vehicle description to text which is easy to parse
std::string toLogString( const orca::VehicleDescription& );
void fromLogString( std::stringstream &, orca::VehicleDescription& );

std::string toLogString( const orca::CameraDescriptionPtr& );
void fromLogString( std::stringstream &, orca::CameraDescription& );

std::string toLogString( const orca::ImageDataPtr& );
void fromLogString( std::stringstream &, orca::ImageData& );

// Multicameras
 std::string toLogString( const orca::MultiCameraDescriptionPtr& );
void fromLogString( std::stringstream &, orca::MultiCameraDescriptionPtr& );

std::string toLogString( const orca::MultiCameraDataPtr& );
void fromLogString( std::stringstream &, orca::MultiCameraDataPtr& );

//}


} // namespace

#endif
