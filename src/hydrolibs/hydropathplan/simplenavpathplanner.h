/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef SIMPLENAVPATHPLANNER_H
#define SIMPLENAVPATHPLANNER_H

#include <hydropathplan/ipathplanner2d.h>
#include <hydropathplan/util.h>
#include <hydroogmap/hydroogmap.h>

namespace hydropathplan {

//!
//! Calculates simple navigation function. Returns false if unsuccessful
//! (algorithm described by Latombe, NF1, ch.4.2.1, p.322)
//!
//! @author Alexei Makarenko, Tobias Kaupp
//!
class SimpleNavPathPlanner : public IPathPlanner2d
{

public: 

    SimpleNavPathPlanner( const hydroogmap::OgMap &ogMap,
                          double robotDiameterMetres,
                          double traversabilityThreshhold,
                          bool   doPathOptimization );

    //!
    //! Warning: may throw 'hydropathplan::Exception's
    //!
    virtual void computePath( int           startX,
                              int           startY,
                              int           endX,
                              int           endY,
                              Cell2DVector &path ) const;

private: 

    void checkInputs( int           startX,
                      int           startY,
                      int           endX,
                      int           endY ) const;

    const hydroogmap::OgMap &ogMap_;

    const double robotDiameterMetres_;
    const double traversabilityThreshhold_;
    const bool   doPathOptimization_;
};

}

#endif
