/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>

#include <QPainter>
#include <orcaobj/orcaobj.h>
#include <orcaice/orcaice.h>
#include <gbxutilacfr/mathdefs.h>
#include <hydroqgui/hydroqgui.h>
#include "localise2dpainter.h"
#include <orcaqgui3d/glutil.h>
#include <osg/ShapeDrawable>
#include <orcaqgui3d/osgutil.h>
#include <orcaifacestring/localise2d.h>

using namespace std;

namespace orcaqgui3d {

Localise2dPainter::Localise2dPainter( bool beginDisplayHistory )
    : basicColor_(Qt::blue),
      isDisplayMultiHypothesis_(true)
{
    root_ = new osg::Group();
}

void
Localise2dPainter::setCubicDescr( double length,
                                  double width,
                                  double height,
                                  const orca::Frame3d &vehicleToGeometryTransform )
{
    cout<<"TRACE(localise2dpainter.cpp): vehicleToGeometryTransform: " << ifacestring::toString(vehicleToGeometryTransform) << endl;

    assert( fabs(vehicleToGeometryTransform.o.r) < 1e-9 &&
            fabs(vehicleToGeometryTransform.o.p) < 1e-9 &&
            fabs(vehicleToGeometryTransform.o.y) < 1e-9 && "Not implemented!" );

    platformNode_ = new osg::PositionAttitudeTransform;
    platformNode_->setPosition( osg::Vec3( vehicleToGeometryTransform.p.x,
                                           vehicleToGeometryTransform.p.y,
                                           vehicleToGeometryTransform.p.z ) );

    osg::ref_ptr<osg::Box> box = new osg::Box( osg::Vec3( 0, 0, 0 ), length, width, height );
    osg::ref_ptr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(box.get());
    drawable->setColor( orcaqgui3d::toVec4( currentColor_ ) );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( drawable.get() );

    platformNode_->addChild( geode.get() );
}

void
Localise2dPainter::setCylindricalDescr( double radius,
                                        double height,
                                        const orca::Frame3d &vehicleToGeometryTransform )
{
    cout<<"TRACE(localise2dpainter.cpp): vehicleToGeometryTransform: " << ifacestring::toString(vehicleToGeometryTransform) << endl;

    assert( fabs(vehicleToGeometryTransform.o.r) < 1e-9 &&
            fabs(vehicleToGeometryTransform.o.p) < 1e-9 &&
            fabs(vehicleToGeometryTransform.o.y) < 1e-9 && "Not implemented!" );

    platformNode_ = new osg::PositionAttitudeTransform;
    platformNode_->setPosition( osg::Vec3( vehicleToGeometryTransform.p.x,
                                           vehicleToGeometryTransform.p.y,
                                           vehicleToGeometryTransform.p.z ) );

    osg::Vec4 color = orcaqgui3d::toVec4( currentColor_ );
    platformNode_->addChild( orcaqgui3d::drawCylinder( height, radius, color ).get() );
}

void
Localise2dPainter::clear()
{
    cout<<"TRACE(localise2dpainter.cpp): TODO: "<<__func__ << endl;
}

void 
Localise2dPainter::setData( const orca::Localise2dData& data )
{    
    // Clear
    root_->removeChildren( 0, root_->getNumChildren() );

    // And re-set
    if ( data.hypotheses.size() != 0 )
    {
        if ( isDisplayMultiHypothesis_ )
        {
            for ( unsigned int i=0; i<data.hypotheses.size(); ++i )
            {
                paintHypothesis( data.hypotheses[i] );
            }
        }
        else
        {
            paintHypothesis( orcaobj::mlHypothesis( data ) );
        }
    }
}

void 
Localise2dPainter::paintHypothesis( const orca::Pose2dHypothesis &hypothesis )
{
    if ( !platformNode_ ) return;

//    const float weight = hypothesis.weight;
    const orca::Frame2d      &mean = hypothesis.mean;
//    const orca::Covariance2d &cov  = hypothesis.cov;

    osg::ref_ptr<osg::PositionAttitudeTransform> pos = new osg::PositionAttitudeTransform;
    pos->setPosition( osg::Vec3( mean.p.x, mean.p.y, 0 ) );
    osg::Quat rotation; rotation.makeRotate( mean.o, osg::Vec3(0,0,1) );
    pos->setAttitude( rotation );

    pos->addChild( platformNode_.get() );
    root_->addChild( pos.get() );


    // TODO: draw uncertainty.

#if 0
    float weight = hypothesis.weight;

    // Don't bother painting _really_ unlikely hypotheses
    // if ( weight < 0.02 ) continue;

    const orca::Frame2d      &mean = hypothesis.mean;
    const orca::Covariance2d &cov  = hypothesis.cov;

    // Translate to where the hypothesis is at
    {
        orcaqgui3d::glutil::ScopedMatrixSave sms;

        // Translate to centre of vehicle coords
        glTranslatef( mean.p.x, mean.p.y, 0.0 );
        glRotatef( RAD2DEG(mean.o), 0.0, 0.0, 1.0 );
        orcaqgui3d::glutil::transform( vehicleToGeometryTransform_.p.x,
                                       vehicleToGeometryTransform_.p.y,
                                       vehicleToGeometryTransform_.p.z,
                                       vehicleToGeometryTransform_.o.r,
                                       vehicleToGeometryTransform_.o.p,
                                       vehicleToGeometryTransform_.o.y );

        float alpha;
        if (useTransparency_)
            alpha = weight;
        else
            alpha = 1.0;
        glColor4f( currentColor_.red(), currentColor_.green(), currentColor_.blue(), alpha );

        // Rotate to draw the platform correctly
        if (platformType_ == PlatformTypeCylindrical)
            paintCylindricalPlatformPose( height_, radius_  );
        else
            paintCubicPlatformPose( length_, width_, weight );
        glColor4f( currentColor_.red(), currentColor_.green(), currentColor_.blue(), alpha );
        cout<<"TRACE(localise2dpainter.cpp): TODO: uncertainty wedge" << endl;
//        hydroqguielementutil::paintUncertaintyWedge( cov.tt );

        // Rotate back to world coords for the uncertainty ellipse
        glRotatef( RAD2DEG(-vehicleToGeometryTransform_.o.y-mean.o), 0.0, 0.0, 1.0 );
        // And down to ground level (plus a bit so we can see the ellipse)
        glTranslatef( 0, 0, -vehicleToGeometryTransform_.p.z+0.01 );

        orcaqgui3d::glutil::paintCovarianceEllipse( cov.xx, cov.xy, cov.yy );
    }
#endif
}

void Localise2dPainter::setColor( QColor color )
{
    basicColor_ = color;
    currentColor_ = color; 
}
    
void Localise2dPainter::setFocus( bool inFocus )
{
    //cout << "TRACE(localise2dpainter.pp): inFocus is " << inFocus << endl;
    if (!inFocus) {
        currentColor_=Qt::gray;
    } else {
        currentColor_=basicColor_;
    }
}

    
}