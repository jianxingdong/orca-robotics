/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <hydroqgui/exceptions.h>
#include "localise2delement.h"
#include <orcaqgui2dfactory/connectutils.h>

using namespace std;

namespace orcaqgui3d {

void
Localise2dElement::actionOnConnection()
{
    if (!isConnected_) return;

    tryToGetGeometry();
    
    orcaqgui2d::paintInitialData<orca::Localise2dPrx, Localise2dPainter>
        ( context_, listener_.interfaceName(), painter_ );
}

void 
Localise2dElement::tryToGetGeometry()
{
    orca::VehicleGeometryDescriptionPtr geom;
    
    if ( listener_.proxy()==0 )
    {
        humanManager_->showStatusMsg(hydroqguielementutil::IHumanManager::Error,"Localise2dElement::tryToGetGeometry(): listener_.proxy()==0"  );
    }
    try 
    {
        geom = listener_.proxy()->getVehicleGeometry();
        if ( geom == 0 )
        {
            throw gbxutilacfr::Exception( ERROR_INFO, "Localise2dElement::tryToGetGeometry(): got NULL vehicle geometry from remote interface!" );
        }
        haveGeometry_ = true;
    }
    catch ( Ice::Exception &e)
    {
        stringstream ss;
        ss << "Localise2dElement::tryToGetGeometry(): Exception when trying to get geometry: " << e;
        humanManager_->showStatusMsg( hydroqguielementutil::IHumanManager::Error, ss.str().c_str() );
    }
    catch ( std::exception &e)
    {
        humanManager_->showStatusMsg( hydroqguielementutil::IHumanManager::Error,"Localise2dElement::tryToGetGeometry(): Exception when trying to get geometry: " + QString(e.what()) );
    }
    
    if (haveGeometry_ && geom->type==orca::VehicleGeometryCuboid)
    {
        orca::VehicleGeometryCuboidDescriptionPtr cuboidGeom = orca::VehicleGeometryCuboidDescriptionPtr::dynamicCast( geom );
        painter_.setCubicDescr( cuboidGeom->size.l,
                                cuboidGeom->size.w,
                                cuboidGeom->size.h,
                                cuboidGeom->vehicleToGeometryTransform );
    }
    else if (haveGeometry_ && geom->type==orca::VehicleGeometryCylindrical)
    {
        orca::VehicleGeometryCylindricalDescriptionPtr cylGeom = orca::VehicleGeometryCylindricalDescriptionPtr::dynamicCast( geom );
        painter_.setCylindricalDescr( cylGeom->radius,
                                      cylGeom->height,
                                      cylGeom->vehicleToGeometryTransform );
    }
    else
    {
        humanManager_->showStatusMsg(hydroqguielementutil::IHumanManager::Warning, "Localise2dElement: unknown geometry!");
        const double length = 1e-3;
        const double width = 1e-3;
        const double height = 1e-3;
        painter_.setCubicDescr( length, width, height, orcaobj::zeroFrame3d() );
    }
}

void
Localise2dElement::update()
{
    // standard update as in IceStormElement3d
    if ( !orcaqgui3d::IceStormElement3d<Localise2dPainter,
            orca::Localise2dData,
            orca::Localise2dPrx,
            orca::Localise2dConsumer,
            orca::Localise2dConsumerPrx>::needToUpdate() )
        return;

    assert( !listener_.buffer().isEmpty() );
    
    listener_.buffer().getAndPop( data_ );
    painter_.setData( data_ );

    // custom update, but first error-check.
    if ( data_.hypotheses.size() == 0 )
    {
        std::stringstream ss;
        ss << "Localise2dElement::update(): Interface " << listener_.interfaceName() << ": Localise2dData had zero hypotheses";
        throw hydroqgui::Exception( ERROR_INFO, ss.str() );
    }
    const orca::Pose2dHypothesis &h = orcaobj::mlHypothesis( data_ );
    
    x_ = h.mean.p.x;
    y_ = h.mean.p.y;
    yaw_ = h.mean.o;
}

QStringList
Localise2dElement::contextMenu()
{
    QStringList s;
    s<<"Toggle History"<<"Toggle Multi-Hypothesis";
    return s;
}

void
Localise2dElement::execute( int action )
{
    switch ( action )
    {
        case 0 :
        {
            painter_.toggleDisplayHistory();
            break;
        }
        case 1 :
        {
            painter_.toggleMultiHypothesis();
            break;
        }
        default:
        {
            throw hydroqgui::Exception( ERROR_INFO, "execute(): What the hell? bad action." );
            break;
        }
    }
}

}
