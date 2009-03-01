/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCAGUI3D_LOCALISE2D_PAINTER_H
#define ORCAGUI3D_LOCALISE2D_PAINTER_H

#include <QColor>
#include <orca/localise2d.h>
#include <hydroqguielementutil/paintutils.h>
#include <osg/Group>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>

// forward declarations
class QPainter;

namespace orcaqgui3d
{
    
class Localise2dPainter
{
  public:
    Localise2dPainter( bool beginDisplayHistory );

    void setData( const orca::Localise2dData& data );
    void setCubicDescr( double length,
                        double width,
                        double height,
                        const orca::Frame3d &platformToGeometryTransform );
    void setCylindricalDescr( double radius,
                              double height,
                              const orca::Frame3d &platformToGeometryTransform );

    void clear();
    void setColor( QColor color );
    void setFocus( bool inFocus );

    void toggleMultiHypothesis() { isDisplayMultiHypothesis_ = !isDisplayMultiHypothesis_; }
    void setUseTransparency(bool u)  { useTransparency_ = u; }

    osg::Node *osgNode() const { return root_.get(); }

  private:

    void drawCuboidPlatform();
    void drawCylindricalPlatform();

    void paintHypothesis( const orca::Pose2dHypothesis &hypothesis );

    QColor basicColor_;
    QColor currentColor_;

    bool useTransparency_;
    bool isDisplayMultiHypothesis_;

    osg::ref_ptr<osg::PositionAttitudeTransform> platformNode_;

    osg::ref_ptr<osg::Group> root_;
};

}

#endif
