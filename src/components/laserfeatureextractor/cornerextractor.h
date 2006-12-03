/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,v
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef CORNEREXTRACTOR__H
#define CORNEREXTRACTOR__H

#include <orca/laserscanner2d.h>
#include <orca/polarfeature2d.h>
#include <vector>
#include "section.h"
#include <orcaice/context.h>
#include "iextractor.h"

namespace laserfeatures {

class CornerExtractor : public IExtractor
{

public: 

    CornerExtractor( orcaice::Context, double laserMaxRange );

    // Adds laser features to the 'features' data structure
    void addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                        orca::PolarFeature2dDataPtr &features );  
    
private: 
    
    std::vector<Section> sections_;
    double laserMaxRange_;
    double minLineLength_;

    // Not used at the moment
    bool extractPossibleCorners( const orca::PolarFeature2dDataPtr & featureDataPtr );
    
    void connectSections(const orca::LaserScanner2dDataPtr & laserDataPtr);
    void extractLines();
    void findBreakPoint(Section &s, double &maxDist, int &pos);
    void fitLine(Section &s);
    void printSections();
        
};

}

#endif
