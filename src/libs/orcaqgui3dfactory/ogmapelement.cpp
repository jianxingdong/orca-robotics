/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "ogmapelement.h"
#include <iostream>
#include <QFileDialog>
#include <hydroqgui/exceptions.h>

using namespace std;

namespace orcaqgui3d {

OgMapElement::OgMapElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                            const orcaice::Context                     &context )
    : orcaqgui3d::IceStormElement3d<OgMapPainter,
                      orca::OgMapData,
                      orca::OgMapPrx,
                      orca::OgMapConsumer,
                      orca::OgMapConsumerPrx>( guiElementInfo, context, painter_, -1 ),
      mapFileName_("/home"),
      mapFileNameSet_(false)
{
}

QStringList
OgMapElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Display"<<"Toggle GroundPlane"<<"Toggle Walls";
    return s;
}

void 
OgMapElement::execute( int action )
{
    switch ( action )
    {
    case 0 :
    {
        painter_.toggleDisplay();
        break;
    }
    case 1 :
    {
        painter_.toggleGroundPlane();
        break;
    }
    case 2 :
    {
        painter_.toggleWalls();
        break;
    }
    default:
    {
        throw hydroqgui::Exception( ERROR_INFO, "OgMapElement::execute(): What the hell? bad action." );
        break;
    }
    }
}

}
