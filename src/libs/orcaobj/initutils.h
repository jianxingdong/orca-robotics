/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_INIT_UTILS_H
#define ORCAOBJ_INIT_UTILS_H

#include <orca/orca.h>

namespace orcaobj
{
/*!
 *  @name Object Initialization Functions
 */
//@{
//! Sets all fields to specified values
void setInit( orca::Frame2d&, double x=0.0, double y=0.0, double orient=0.0 );
//! Sets all fields to specified values
void setInit( orca::Frame3d&, double x=0.0, double y=0.0, double z=0.0, double pitch=0.0, double roll=0.0, double yaw=0.0 );
//! Sets all fields to specified values
void setInit( orca::Twist2d&, double vx=0.0, double vy=0.0, double w=0.0 );
//! Sets all fields to specified values
void setInit( orca::Twist3d&, double vx=0.0, double vy=0.0, double vz=0.0, double wx=0.0, double wy=0.0, double wz=0.0 );
//! Sets all fields to specified values
void setInit( orca::Size2d&, double l=0.0, double w=0.0 );
//! Sets all fields to specified values
void setInit( orca::Size3d&, double l=0.0, double w=0.0, double h=0.0 );
//! Sets all fields to specified values
void setInit( orca::CartesianPoint2d&, double x=0.0, double y=0.0 );
//! Sets all fields to specified values
void setInit( orca::CartesianPoint&, double x=0.0, double y=0.0, double z=0.0 );
//! Sets all fields to specified values
void setInit( orca::Covariance2d&, double pxx=0.0, double pxy=0.0, double pyy=0.0, double pxt=0.0, double pyt=0.0, double ptt=0.0 );
//! Sets all fields to zero.
void setInit( orca::Time& );
// alexm: sparing it for now because it has a non-zero entry.
//! Sets all fields to zero
void setInit( orca::WifiInterface &w );

// OBSOLETE

/*
//! Sets all fields to zero
void setInit( orca::Waypoint2d& );
//! Sets all fields to zero and stalled to FALSE.
void setInit( orca::Odometry2dData& );
//! Sets all fields to negative numbers
void setInit( orca::BatteryData& );
*/
/*
//! Set to randomized but sane values.
void setSane( orca::CartesianPoint2d& );
//! Set to randomized but sane values.
void setSane( orca::CartesianPoint& );
//! Set to randomized but sane values.
void setSane( orca::CartesianVelocity2d& );
//! Set to randomized but sane values.
void setSane( orca::CartesianVelocity& );
//! Set to randomized but sane values.
void setSane( orca::AngularVelocity& );
//! Sets to current time.
void setSane( orca::Date& );
//! Set to randomized but sane values.
void setSane( orca::Frame2d& );
//! Set to randomized but sane values.
void setSane( orca::Frame3d& );
//! Set to randomized but sane values.
void setSane( orca::Size2d& );
//! Set to randomized but sane values.
void setSane( orca::Size3d& );
//! Set to randomized but sane values.
void setSane( orca::Covariance2d& );
//! Sets to current time.
void setSane( orca::Time& );
//! Sets to current time.
void setSane( orca::TimeOfDay& );

//! Set to randomized but sane values.
void setSane( orca::CameraData&, int width=640, int height=480 );
//! Set to randomized but sane values.
void setSane( orca::DriveBicycleCommand& );
//! Set to randomized but sane values.
void setSane( orca::DriveBicycleData& );
//! Set to randomized but sane values.
void setSane( orca::GpsData& );
//! Set to randomized but sane values.
void setSane( orca::Localise2dData& );
//! Set to randomized but sane values.
void setSane( const orca::RangeScanner2dDataPtr&, int numberOfSamples=361 );
//! Set to randomized but sane values.
void setSane( const orca::LaserScanner2dDataPtr&, int numberOfSamples=361 );
//! Set to randomized but sane values.
void setSane( orca::Odometry2dData& );
//! Set to randomized but sane values.
void setSane( orca::OgMapData&, int width=640, int height=480 );
//! Set to randomized but sane values.
void setSane( orca::PixMapData&, int width=640, int height=480 );
//! Set to randomized but sane values.
void setSane( orca::PowerData&, int count=2 );
//! Set to randomized but sane values.
void setSane( orca::VelocityControl2dData& );
*/

//@}

//@}


} // namespace

#endif
