/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_GRIDELEMENT3D_H
#define ORCAGUI3D_GRIDELEMENT3D_H

#include <orcaqgui3d/guielement3d.h>
#include <GL/gl.h>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgText/Text>
#include <osg/PositionAttitudeTransform>

namespace orcaqgui3d
{

/*!
  @brief Paints a rectangular grid in the 3d display.
  @author Alex Brooks
*/  
class GridElement : public GuiElement3d
{
public:

    GridElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                 double                                      wireGridSpacing          = 2,
                 double                                      groundPlaneSquareSpacing = 1 );

    virtual bool isInGlobalCS() { return true; }
    
    virtual void setCameraPose( const orcaqgui3d::CoordinateFrame &cameraPose );
    virtual QStringList contextMenu();
    virtual void execute( int action );
    
    osg::Node *osgNode() const { return root_.get(); }

private:

    void drawGroundPlane();
    void drawWireGrid();
    // void drawLabels();
    void drawOrigin();
    
    double wireGridSpacing_;
    double groundPlaneSquareSpacing_;

    osg::ref_ptr<osg::PositionAttitudeTransform> viewOffset_;
    osg::ref_ptr<osg::Geode> groundPlaneGeode_;
    osg::ref_ptr<osg::Geode> wireGridGeode_;

    osg::ref_ptr<osg::Geode> originGeode_;

    osg::ref_ptr<osg::Group> root_;
};

} // namespace

#endif
