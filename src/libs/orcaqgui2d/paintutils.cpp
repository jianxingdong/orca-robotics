/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <hydroutil/mathdefs.h>
#include <orcaqgui/exceptions.h>
#include "detail/cov2d.h"
#include "paintutils.h"

using namespace std;
using namespace orcaqgui;

////////////////////////////////////////////////////////////////////////////////

// STATIC VARIABLES

// How much uncertainty to draw
const int N_SIGMA = 3;
    
////////////////////////////////////////////////////////////////////////////////

namespace orcaqgui2d {

    namespace {

        // Minimum change required for the addition of a new point
        const double POSE_HISTORY_MIN_DISTANCE = 0.250;    
        // Maximum change to qualify as a new point
        // This is to prevent 'teleporting'
        const double POSE_HISTORY_MAX_DISTANCE = 8.0;

        // Limit on the size of the history array
        const int POSE_HISTORY_MAX_LENGTH = 1000;
    }

PoseHistory::PoseHistory( double lineThickness )
    : lineThickness_(lineThickness),
      totalPoints_(0)
{}
    
void
PoseHistory::addPoint( const double x, const double y )
{
    if ( histories_.isEmpty() )
    {
        QPolygonF history;
        history.push_back( QPointF(x,y) );
        totalPoints_++;
        histories_.push_back( history );
    }
    
    const QPointF last = histories_.last().last();
    double dist = hypotf( last.x()-x, last.y()-y );
    
    if ( dist > POSE_HISTORY_MAX_DISTANCE )
    {
        // start a new history
        QPolygonF history;
        history.push_back( QPointF(x,y) );
        totalPoints_++;
        histories_.push_back( history );
    }
    else
    {
        histories_.last().push_back( QPointF(x,y) );  
        totalPoints_++;  
    }
    
    if (totalPoints_>POSE_HISTORY_MAX_LENGTH)
    {
        // take a point from the beginning off
        histories_.first().remove( 0,1 );
        if (histories_.first().size()==0)
            histories_.remove( 0,1 );
    }
}

void
PoseHistory::paint( QPainter *p, const QColor &colour )
{
    QMatrix m = p->worldMatrix();

    for (int i=0; i<histories_.size(); i++)
    {
        p->setPen( QPen( colour, lineThickness_/m.m11() ) );
        p->drawPolyline( histories_[i] );
    }
}

void
paintOrigin( QPainter *p, const QColor &colour, float radius, float lineThickness  )
{
    const double cross = 2*radius;
    p->setBrush( QBrush() );
    p->setPen( QPen( colour, lineThickness ) );

    // display origin
    p->drawEllipse( QRectF(-radius,-radius, 2.0*radius, 2.0*radius) );
    p->drawLine( QPointF(0.0,0.0), QPointF(cross,0.0) );
    p->drawLine( QPointF(0.0,0.0), QPointF(0.0,cross) );
    
    p->setFont( QFont("Helvetica [Cronyx]", 12) );
    
    QPointF xlabel = QPointF(cross,0.0) * p->worldMatrix();  // x-label position in window cs
    QPointF ylabel = QPointF(0.0,cross) * p->worldMatrix();  // y-label position in window cs

    p->setWorldMatrix( QMatrix() );
    
    p->drawText( xlabel, "x" );
    p->drawText( ylabel, "y" );
}

void paintCylindricalPlatformPose( QPainter *p, const QColor &colour, float radius, float transparencyMultiplier, float minRadius, float lineThickness )
{
    if (radius < minRadius)
    {
       radius = minRadius;
    }
    p->setBrush( colour );
    p->setPen( QPen( Qt::black, lineThickness ) );
    p->drawEllipse( QRectF( -radius, -radius, 2*radius, 2*radius) );

    // 30 degrees either side
    const double lineLengthX = radius * std::sqrt(3)/2.0;
    const double lineLengthY = radius * 0.5;
    p->drawLine( QPointF(0.0,0.0), QPointF(lineLengthX,-lineLengthY) );
    p->drawLine( QPointF(0.0,0.0), QPointF(lineLengthX, lineLengthY) );
}

void paintCubicPlatformPose( QPainter *p, const QColor &colour, float length, float width, float transparencyMultiplier, float minLength, float lineThickness )
{
    if (length < minLength)
    {
       width *= minLength/length;
	   length = minLength;
    }

    double halfWidth = width/2.0;
    double halfLength = length/2.0;

    // paint body
    p->setBrush( colour );
    // The outline
    p->setPen( QPen( Qt::black, lineThickness ) );
    
    p->drawRect( QRectF( -halfLength, -halfWidth, length, width) ); // top,left, width,height
    p->drawLine( QPointF(0.0,0.0), QPointF(halfLength,halfWidth) );
    p->drawLine( QPointF(0.0,0.0), QPointF(halfLength,-halfWidth) );

}

void
paintUncertaintyInfo( QPainter *p,
                      const QColor &colour,
                      float pxx,
                      float pxy,
                      float pyy,
                      float ptt,
                      float length,
                      float lineThickness )
{
    paintUncertaintyWedge( p, colour, ptt, length, lineThickness );
    paintCovarianceEllipse( p, colour, pxx, pxy, pyy, lineThickness );
}

void
paintUncertaintyWedge( QPainter *p,
                       const QColor &colour,
                       float ptt,
                       float length,
                       float lineThickness)
{
        p->setPen( QPen(colour, lineThickness) );
        p->setBrush( Qt::NoBrush );
        int pTheta = (int) (ptt * (180.0/M_PI));
        p->drawPie( QRectF(-length,-length,2.0*length,2.0*length),
                    -N_SIGMA*pTheta*16, 2*N_SIGMA*pTheta*16 );
}

void
paintCovarianceEllipse( QPainter *p, const QColor &colour, float pxx, float pxy, float pyy, float lineThickness )
{    
    // Quick checks first (note that this is a necessary but not
    // sufficient condition for positive-definiteness)
    if ( pxx < 0.0 ||
         pyy < 0.0 ||
         fabs(pxy) >= sqrt( pxx*pyy ) ||
         (isnan(pxx)||isinf(pxx))    ||
         (isnan(pxy)||isinf(pxy))    ||
         (isnan(pyy)||isinf(pyy))    )
    {
        std::stringstream ss;
        ss << "paintCovarianceEllipse(): covariance matrix not PD: pxx,pxy,pyy = "<<pxx<<","<<pxy<<","<<pyy;
//         ss << endl << "fabs(pxy)-sqrt(pxx*pyy): " << fabs(pxy)<< "-" <<sqrt(pxx*pyy)<<" = "<< fabs(pxy)-sqrt(pxx*pyy);
//         ss << endl << "fabs(pxy) > sqrt( pxx*pyy ): " << (fabs(pxy) > sqrt( pxx*pyy ));
//         ss << endl << "isnan||isinf(pxx): " << (isnan(pxx)||isinf(pxx));
//         ss << endl << "isnan||isinf(pxy): " << (isnan(pxy)||isinf(pxy));
//         ss << endl << "isnan||isinf(pyy): " << (isnan(pyy)||isinf(pyy));
        throw orcaqgui::Exception( ss.str() );
    }

    // Work out the details of the uncertainty ellipse
    double a, b, psi;
    Cov2d cov( pxx, pxy, pyy );
    cov.ellipse( a, b, psi );

    QPointF covPoint(a,b);
    float psiDeg = (int)ceil( psi *180/M_PI );            // rot. angle into princ. axes

    // Paint uncertainty ellipse
    p->save();
    {
        p->rotate( psiDeg );
        p->setPen( QPen( colour, lineThickness ) );
        p->setBrush( Qt::NoBrush );
        p->drawEllipse( QRectF( -N_SIGMA*a,
                                -N_SIGMA*b,
                                2*N_SIGMA*a,
                                2*N_SIGMA*b ) );
    }
    p->restore();
}


QColor
getTransparentVersion( const QColor &c, float transparencyMultiplier )
{
    // Lower alpha gives a more transparent version.
    int alpha = (int) (c.alpha()*transparencyMultiplier);
    return QColor( c.red(),
                   c.green(),
                   c.blue(),
                   alpha );
}

QColor 
getDarkVersion( const QColor &color )
{
    return QColor(color.red()/2,color.green()/2,color.blue()/2);
}

void paintWaypoint( QPainter *p,
                    const QColor &fillColor,
                    const QColor &drawColor,
                    int   targetHeading,
                    float distanceTolerance,
                    int   headingTolerance )
{
    p->setPen( QPen( drawColor, 0.1 ) );
    p->drawEllipse( QRectF( -distanceTolerance,
                      -distanceTolerance,
                      2*distanceTolerance,
                      2*distanceTolerance) );       // draws circle as tolerances
    p->setBrush( fillColor );
    int a = -(targetHeading-headingTolerance);
    int alen = -2*headingTolerance;
    // !!!!!!!!!! WARNING!!!!!!!!!!!!!!!!!!!!
    // Qt documentation is wrong: positive is clockwise and negative is counterclockwise
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    p->drawPie( QRectF( -distanceTolerance, -distanceTolerance, 2*distanceTolerance, 2*distanceTolerance ),
                a, 
                alen );
}

} // namespace
