#include "testsimutil.h"
#include <iostream>
#include <hydroutil/rand.h>
#include <orcaice/orcaice.h>

using namespace std;

namespace orcalocalnavtest {

namespace {

    const double ROOM_SPACE = 12.0;

    void addTurnOnSpot( orca::Waypoint2d         &wp,
                        orca::PathFollower2dData &d,
                        int                      &t,
                        bool                      stressTiming )
    {
        if ( stressTiming )
            t+=3;
        wp.target.p.x   = 0;
        wp.target.p.y   = 0;
        wp.target.o     = M_PI;
        wp.timeTarget   = t;
        d.path.push_back( wp );

        if ( stressTiming )
            t+=3;
        wp.target.p.x   = 0;
        wp.target.p.y   = 0;
        wp.target.o     = -M_PI/2;
        wp.timeTarget   = t;
        d.path.push_back( wp );

        if ( stressTiming )
            t+=3;
        wp.target.p.x   = 0;
        wp.target.p.y   = 0;
        wp.target.o     = M_PI/2;
        wp.timeTarget   = t;
        d.path.push_back( wp );

        if ( stressTiming )
            t+=3;
        wp.target.p.x   = 0;
        wp.target.p.y   = 0;
        wp.target.o     = M_PI/2;
        wp.timeTarget   = t;
        d.path.push_back( wp );        
    }

    void addRegularWaypointGrid( orca::Waypoint2d         &wp,
                                 orca::PathFollower2dData &d,
                                 int                      &t,
                                 bool                      stressTiming )
    {
        for ( double x = 0; x <= 7.6; x += 3.3 ) // /2.0 )
        {
            if ( stressTiming )
                t+=3;
            wp.target.p.x   = x;
            wp.target.p.y   = hydroutil::randNum(-1,1);
            wp.target.o     = 0;
            wp.timeTarget   = t;
            d.path.push_back( wp );
        }
        for ( double x = 6.6; x >= -7.6; x -= 3.3 ) // /2.0 )
        {
            if ( stressTiming )
                t+=3;
            wp.target.p.x   = x;
            wp.target.p.y   = hydroutil::randNum(-4.3,-2.3);
            wp.target.o     = 0;
            wp.timeTarget   = t;
            d.path.push_back( wp );
        }
    }

    void addGoThruDoor( orca::Waypoint2d         &wp,
                        orca::PathFollower2dData &d,
                        int                      &t,
                        bool                      stressTiming )
    {
        const double dt = 10;

        // move off
        if ( stressTiming )
            t+=dt;
        wp.target.p.x   = 8;
        wp.target.p.y   = 8;
        wp.target.o     = M_PI;
        wp.timeTarget   = t;
        d.path.push_back( wp );

        // force a change in speed
        if ( stressTiming )
            t+=dt;
        wp.target.p.x   = 10;
        wp.target.p.y   = 10;
        wp.target.o     = M_PI;
        wp.timeTarget   = t;
        d.path.push_back( wp );

        if ( stressTiming )
            t+=dt;
        wp.target.p.x   = 10;
        wp.target.p.y   = 0;
        wp.target.o     = M_PI;
        wp.timeTarget   = t;
        d.path.push_back( wp );

        // Try to get thru door
        if ( stressTiming )
            t+=dt;
        wp.target.p.x   = -14;
//        wp.target.p.y   = 10;
        wp.target.p.y   = 9;
        wp.target.o     = M_PI/2.0;
        wp.timeTarget   = t;
        d.path.push_back( wp );

        if ( stressTiming )
            t+=dt;
//        wp.target.p.x   = -14;
        wp.target.p.x   = -14.5;
//        wp.target.p.x   = -15;
        wp.target.p.y   = 14;
        wp.target.o     = M_PI/2.0;
        wp.timeTarget   = t;
        d.path.push_back( wp );
        
        // Then back out.
        if ( stressTiming )
            t+=dt;
        wp.target.p.x   = -14;
        wp.target.p.y   = 14;
        wp.target.o     = -M_PI/2.0;
        wp.timeTarget   = t;
        d.path.push_back( wp );

        if ( stressTiming )
            t+=dt;
        wp.target.p.x   = -14;
        wp.target.p.y   = 10;
        wp.target.o     = -M_PI/2.0;
        wp.timeTarget   = t;
        d.path.push_back( wp );        
    }

    bool isInRoomSpace( double x,
                        double y,
                        const hydroogmap::OgMap &ogMap )
    {
        return ( x < -ogMap.worldSizeX()/2.0+ROOM_SPACE &&
                 y > ogMap.worldSizeY()/2.0-ROOM_SPACE );
    }

    bool isNearObstacle( const orca::Frame2d     &p,
                         const hydroogmap::OgMap &ogMap )
    {
        double prox = 0.3;
        for ( double x = p.p.x-prox; x <= p.p.x+1.05*prox; x+=prox/2.0 )
        {
            for ( double y = p.p.y-prox; y <= p.p.y+1.05*prox; y+=prox/2.0 )
            {
                if ( !ogMap.coordsWithinMap( x, y ) ) return true;
                if ( !hydroogmap::isTraversable( ogMap.worldCell(x,y) ) ) return true;
            }
        }
        return false;
    }
}

orca::PathFollower2dData
getTestPath( const hydroogmap::OgMap &ogMap,
             int numWaypoints,
             bool stressTiming,
             bool regularWaypointGrid,
             bool turnOnSpot, 
             bool goThruDoor )
{
    orca::PathFollower2dData d;

    d.timeStamp = orcaice::getNow();
    orca::Waypoint2d wp;
    int t=0;

    // These settings will be held constant unless otherwise specified.
//    wp.distanceTolerance = 1.5;
    wp.distanceTolerance = 0.8;
//    wp.headingTolerance  = M_PI/8.0;
    wp.headingTolerance  = 2.0*M_PI;
    wp.maxApproachSpeed    = 3.0;
    wp.maxApproachTurnrate = 90*M_PI/180.0;

    if ( turnOnSpot )
    {
        addTurnOnSpot( wp, d, t, stressTiming );
    }

    if ( goThruDoor )
    {
        addGoThruDoor( wp, d, t, stressTiming );
    }

    if ( regularWaypointGrid )
    {
        addRegularWaypointGrid( wp, d, t, stressTiming );
    }

    // Now add a random marathon
    int numRandom=numWaypoints-d.path.size();
    for ( int i=0; i < numRandom; i++ )
    {
        wp.target.p.x   = hydroutil::randNum( ogMap.offset().p.x,
                                              ogMap.offset().p.x+ogMap.worldSizeX() );
        wp.target.p.y   = hydroutil::randNum( ogMap.offset().p.y,
                                              ogMap.offset().p.y+ogMap.worldSizeY() );
        wp.target.o     = hydroutil::randNum(-M_PI,M_PI);

        if ( goThruDoor && 
             isInRoomSpace( wp.target.p.x, wp.target.p.y, ogMap ) )
        {
            // Don't put a random wp here.
            i--;
            continue;
        }
        else if ( isNearObstacle( wp.target, ogMap ) )
        {
            // Don't put a random wp here.
            i--;
            continue;            
        }

        if ( stressTiming )
            t += (int) hydroutil::randNum(3,10);
        wp.timeTarget = t;
        wp.maxApproachSpeed = hydroutil::randNum( 1.0, 5.0 );
        wp.maxApproachTurnrate = hydroutil::randNum( 45*M_PI/180.0, 180*M_PI/180.0 );
        d.path.push_back( wp );
    }

    if ( (int)d.path.size() != numWaypoints )
    {
        stringstream ss;
        ss << "Asked for numWaypoints="<<numWaypoints<<", but d.path.size() = " << d.path.size();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    return d;
}

namespace {
    
    void
    placeRoom( hydroogmap::OgMap &ogMap )
    {
        double centreX = -14, centreY = 14;
//        double widthX = 4, widthY = 4;
        double widthX = 2, widthY = 4;
//        double doorWidth = 1.0;
        double doorWidth = 0.8;

        // the corners in grid-coords
        int blX, blY;
        int tlX, tlY;
        int trX, trY;
        int brX, brY;
        ogMap.getCellIndices( centreX-widthX/2, centreY-widthY/2, blX, blY );
        ogMap.getCellIndices( centreX-widthX/2, centreY+widthY/2, tlX, tlY );
        ogMap.getCellIndices( centreX+widthX/2, centreY-widthY/2, brX, brY );
        ogMap.getCellIndices( centreX+widthX/2, centreY+widthY/2, trX, trY );

        // side walls
        for ( int yi=blY; yi <= tlY; yi++ )
        {
            ogMap.gridCell(blX,yi) = hydroogmap::CELL_OCCUPIED;
            ogMap.gridCell(brX,yi) = hydroogmap::CELL_OCCUPIED;
        }
        // top & bottom walls
        for ( int xi=blX; xi <= brX; xi++ )
        {
            ogMap.gridCell(xi,tlY) = hydroogmap::CELL_OCCUPIED;
            ogMap.gridCell(xi,brY) = hydroogmap::CELL_OCCUPIED;
        }

        // clear a doorway
        int doorLeftX, doorRightX, doorY;
        ogMap.getCellIndices( centreX-doorWidth/2, centreY-widthY/2, doorLeftX, doorY );
        ogMap.getCellIndices( centreX+doorWidth/2, centreY-widthY/2, doorRightX, doorY );

        for ( int xi=doorLeftX; xi <= doorRightX; xi++ )
            ogMap.gridCell(xi,doorY) = hydroogmap::CELL_EMPTY;
    }

}

hydroogmap::OgMap setupMap( double worldSize, 
                            double cellSize,
                            int    numObstacles,
                            bool   useRoom )
{
    cout<<"TRACE(testsimutil.cpp): "<<__func__<<": useRoom: " << useRoom << endl;

    if ( useRoom )
    {
        assert( worldSize > 2*ROOM_SPACE );
    }

    hydroogmap::OgMap ogMap;
    ogMap.offset().p.x = -worldSize/2.0;
    ogMap.offset().p.y = -worldSize/2.0;
    ogMap.offset().o   = 0.0;
    
    ogMap.setMetresPerCell( cellSize );
    ogMap.reallocate( (int)(worldSize/cellSize), (int)(worldSize/cellSize) );

    ogMap.fill( 0 );

    for ( int i=0; i < numObstacles; i++ )
    {
        double x = hydroutil::randNum( -worldSize/2.0, worldSize/2.0 );
        double y = hydroutil::randNum( -worldSize/2.0, worldSize/2.0 );

        if ( useRoom &&
             isInRoomSpace( x, y, ogMap ) )
        {
            i--;
            continue;
        }
        const double obsRadius = 0.5;
        // Don't allow an obstacle on the origin
        if ( fabs(x) < 4*obsRadius &&
             fabs(y) < 4*obsRadius )
        {
            i--;
            continue;
        }
        hydrosim2d::placeSquareObstacle( ogMap, x, y, obsRadius*2.0 );
    }

    if ( useRoom )
        placeRoom( ogMap );

    return ogMap;
}

void checkProgress( const orca::PathFollower2dData     &path, 
                    const hydrosim2d::VehicleSimulator &vehicleSim,
                    int                                 iterationNum,
                    int                                 iterationLimit,
                    int                                &wpI,
                    bool                               &pathCompleted,
                    bool                               &pathFailed )
{
    if ( wpI == (int)(path.path.size()) )
    {
        pathCompleted = true;
        pathFailed    = false;
        return;
    }

    assert( path.path.size() > 0 );
    assert( wpI < (int)(path.path.size()) );

    pathCompleted = false;
    pathFailed    = false;

    hydronavutil::Pose pose = vehicleSim.pose();

    // Increment the waypoint index
    while ( true )
    {
        const orca::Waypoint2d &wp = path.path[wpI];

        double distanceToGoal = hypot( pose.y()-wp.target.p.y,
                                       pose.x()-wp.target.p.x );
        double angleToGoal = pose.theta()-wp.target.o;
        NORMALISE_ANGLE( angleToGoal );
        angleToGoal = fabs( angleToGoal );
        bool goalReached = ( distanceToGoal <= wp.distanceTolerance &&
                             angleToGoal <= wp.headingTolerance );

        if ( goalReached )
        {
            wpI++;
            if ( wpI == (int)(path.path.size()) )
            {
                cout<<"TRACE(testsimutil.cpp): pathCompleted in " << iterationNum << " iterations." << endl;
                pathCompleted = true;
                break;
            }
        }
        else
        {
            break;
        }
    }

    const int MIN_NUM_ITERATIONS = 5;
    if ( pathCompleted && iterationNum < MIN_NUM_ITERATIONS )
    {
        cout << "ERROR(simulator.cpp): Huh? How did we reach the goal in only "<< iterationNum << " iterations??" << endl;
        exit(1);
    }

    int maxNumIterations = path.path.size()*3000;
    if ( iterationLimit < maxNumIterations )
        maxNumIterations = iterationLimit;
    if ( iterationNum >= maxNumIterations )
    {
        cout << "ERROR(simulator.cpp): Failed: didn't reach the goal in " <<  iterationNum << " iterations." << endl;
        pathFailed = true;
    }

    // Check for collisions
    if ( vehicleSim.isCollided() )
    {
        cout << "ERROR(simulator.cpp): Collision!!" << endl;
        pathFailed = true;
    }
}

}
