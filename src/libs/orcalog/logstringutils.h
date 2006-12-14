/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCALOG_STRINGIFY_UTILITIES_H
#define ORCALOG_STRINGIFY_UTILITIES_H

#include <string>
#include <orca/ocm.h>
#include <orca/orca.h>
#include <orca/binaryswitch.h>
#include <orca/camera.h>
#include <orca/cpu.h>
#include <orca/gps.h>
#include <orca/ogmap.h>
#include <orca/home.h>
#include <orca/imu.h>
#include <orca/laserscanner2d.h>
#include <orca/position2d.h>
#include <orca/position3d.h>
#include <orca/localise2d.h>
#include <orca/platform2d.h>
#include <orca/power.h>
#include <orca/status.h>
//#include <orca/polarfeature2d.h>
#include <orca/featuremap2d.h>
#include <orca/pathfollower2d.h>
#include <orca/pathplanner2d.h>

namespace orcalog
{
/*!
 *  @name Stringify Orca structures for logging.
 */
//@{

//! Prints out laser data to text which is easy to parse
std::string toLogString( const orca::CpuDataPtr & );
//! Prints out laser data to text which is easy to parse
std::string toLogString( const orca::LaserScanner2dDataPtr & );
//! Prints out localise2d data to text which is easy to parse
std::string toLogString( const orca::Localise2dDataPtr & );
//! Prints out resulting fine path to text in a parsable format
std::string toLogString( const orca::PathPlanner2dDataPtr & );
//! Prints out task path (coarse) to text in a parsable format
std::string toLogString( const orca::PathPlanner2dTaskPtr & );
// //! Prints out polarfeature2d data to text which is easy to parse
// std::string toLogString( const orca::PolarFeature2dDataPtr & );
//! Prints out position2d data to text which is easy to parse
std::string toLogString( const orca::Position2dDataPtr & );
//! Prints out position3d data to text which is easy to parse
std::string toLogString( const orca::Position3dDataPtr & );


//! Prints out Power data to text which is easy to parse
std::string toLogString( const orca::PowerDataPtr & );
//! Parses a line of ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream & stream, orca::PowerDataPtr & );

//! Returns string in simple format "sec usec" 
std::string toLogString( const orca::Time & );
//! Parses a line of ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream & stream, orca::Time & );
//}


} // namespace

#endif
