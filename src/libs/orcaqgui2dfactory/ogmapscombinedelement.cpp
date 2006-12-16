/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "ogmapscombinedelement.h"
#include <orcaogmap/orcaogmap.h>
#include <iostream>
#include <QFileDialog>
#include <orcaqgui/exceptions.h>

using namespace std;
using namespace orca;

namespace orcaqgui {

OgMapsCombinedElement::OgMapsCombinedElement( const orcaice::Context  &context,
                                              const QStringList       &proxyStrList)
    : context_(context),
      isConnected_(false)
{
    
    assert(proxyStrList.size()==2 && "Only 2 OgMaps supported at this point");
    
    cout << "TRACE(ogmapscombinedelement.cpp): Constructor" << endl;
    for (int i=0; i<proxyStrList.size(); i++)
    {
        cout << "proxyString: " << proxyStrList[i].toStdString() << endl;
        IceStormListener<OgMapData,OgMapDataPtr,OgMapPrx,OgMapConsumer,OgMapConsumerPrx>
            *listener = new IceStormListener<OgMapData,
                                             OgMapDataPtr,
                                             OgMapPrx,
                                             OgMapConsumer,
                                             OgMapConsumerPrx>(context, proxyStrList[i].toStdString());
        listeners_.push_back(listener);
    }
}

bool OgMapsCombinedElement::needToUpdate()
{
    for (unsigned int i=0; i<listeners_.size(); i++)
    {
        if ( !listeners_[i]->buffer().isEmpty() )
        {
            // An object has arrived in one of the buffers.  We need to update.
            return true;
        }
    }

    if ( !isConnected_ )
    {
        for (unsigned int i=0; i<listeners_.size(); i++)
        { 
            if ( listeners_[i]->connect() == 0 )
            {
                actionOnConnection();
                isConnected_ = true;
            }
            else
            {
                return false;
            }
        }
    }
    
    return false;
}

void OgMapsCombinedElement::update()
{
    if ( !needToUpdate() ) {
        return;
    }

//     OgMapDataPtr data;
//     for (unsigned int i=0; i<listeners_.size(); i++)
//     {
//         // get data from the buffer
//         listeners_[i]->buffer().getAndPop( data );
//     }
    
    OgMapDataPtr data0;
    listeners_[0]->buffer().getAndPop( data0 );
    OgMapDataPtr data1;
    listeners_[1]->buffer().getAndPop( data1 );
    
    // transfer data into painter
    painter_.setData( data0, data1 );
}


void OgMapsCombinedElement::actionOnConnection()
{
    assert(listeners_.size()>0);
    try 
    {
        OgMapPrx prx;
        cout<< "TRACE(actionOnConnection.cpp): connect to interface " << listeners_[0]->interfaceName() << endl;
        orcaice::connectToInterfaceWithString( context_, prx, listeners_[0]->interfaceName() );
        OgMapDataPtr data0 = prx->getData();
        
        cout<< "TRACE(actionOnConnection.cpp): connect to interface " << listeners_[1]->interfaceName() << endl;
        orcaice::connectToInterfaceWithString( context_, prx, listeners_[1]->interfaceName() );
        OgMapDataPtr data1 = prx->getData(); 
        
        painter_.setData( data0, data1 );
        
            
//         for (unsigned int i=0; i<listeners_.size(); i++)
//         {
//             OgMapPrx prx;
//             cout<< "TRACE(actionOnConnection.cpp): connect to interface " << listeners_[i]->interfaceName() << endl;
//             orcaice::connectToInterfaceWithString( context_, prx, listeners_[i]->interfaceName() );
// //             fuse( prx->getData() );
//         }
//         cout << "painter_->setData" << endl;
//         painter_.setData( totalMap_ );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        std::cout<<"TRACE(actionOnConnection.cpp): Caught exception: " << e << std::endl;
    }
    catch ( Ice::Exception &e )
    {
        std::cout<<"TRACE(actionOnConnection.cpp): Caught some ice exception: " << e << std::endl;
    }
    catch ( std::exception &e )
    {
        std::cout<<"TRACE(actionOnConnection.cpp): Caught some std exception: " << e.what() << std::endl;
    }
    catch ( ... )
    {
        std::cout<<"TRACE(actionOnConnection.cpp): Caught some other exception" << std::endl;
    }
}

QStringList
OgMapsCombinedElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Map";
    return s;
}

void 
OgMapsCombinedElement::execute( int action )
{
    switch ( action )
    {
        case 0 :
        {
            painter_.toggleDisplayMap();
            break;
        }
        default:
        {
            throw orcaqgui::Exception( "OgMapsCombinedElement::execute(): What the hell? bad action." );
            break;
        }
    }
}

}
