#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <orcaogmap/orcaogmap.h>
#include <orcanavutil/orcanavutil.h>
#include <orcaifaceimpl/laserscanner2diface.h>
#include <orcaifaceimpl/localise2diface.h>
#include <orcaifaceimpl/ogmapiface.h>
#include <orca/velocitycontrol2d.h>
#include <orca/odometry2d.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/proxy.h>
#include <orcaice/context.h>
#include <orca/pathfollower2d.h>

namespace localnav {

//!
//! @author Alex Brooks
//!
class Simulator
{

public: 

    Simulator( const orcaice::Context &context,
               const orca::PathFollower2dData &testPath );
    ~Simulator();

    // This is the trigger to advance the simulator one step.
    void setCommand( const orca::VelocityControl2dData &cmd );

    // These can be given out to others: the simulator 
    // will put new data in them on each step.
    orcaice::Proxy<orca::RangeScanner2dDataPtr> obsProxy_;
    orcaice::Proxy<orca::Localise2dData>        locProxy_;
    orcaice::Proxy<orca::Odometry2dData>        odomProxy_;

    void printState();

    orca::VehicleDescription        getVehicleDescription() const;
    
    const orca::RangeScanner2dDescription& rangeScanner2dDescription() const
        { return scannerDescr_; }
    
private: 

    void setupMap();
    void setupInterfaces();
    void applyCurrentVelocity();
    void getRanges();
    void fillPipes();
    void checkProgress();
    // get simulation time
    orca::Time now();

    orcaogmap::OgMap       ogMap_;
    orcaogmap::OgMap       grownOgMap_;
    orcaogmap::OgLosTracer rayTracer_;

    orcanavutil::Pose      pose_;
    double                 velLin_;
    double                 velRot_;

    orcaifaceimpl::LaserScanner2dIface *laserInterface_;
    orcaifaceimpl::Localise2dIface     *localiseInterface_;
    orcaifaceimpl::OgMapIface          *ogMapInterface_;

    orca::LaserScanner2dDataPtr     scan_;
    orca::RangeScanner2dDescription scannerDescr_;

    orca::PathFollower2dData testPath_;

    int iterationNum_;

    bool batchMode_;
    
    orcaice::Context context_;
};

}

#endif
