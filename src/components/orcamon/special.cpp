/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>

#include <orcaice/orcaice.h>
#include <orcaice/printingconsumerI.h>

#include <orca/gps.h>

// #include "subscriber.h"
#include "special.h"

using namespace std;
using namespace orca;

// template <> void attach<GpsPrx,GpsMapGridConsumerPrx,GpsMapGridConsumer,GpsMapGridDataPtr>
void attachGpsMapGrid( const orcaice::Context & context, const std::string & proxyString  )
{
    // Connect directly to the interface
    GpsPrx objectPrx;
    int count = 3;
    while( count )
    {
        try
        {
            orcaice::connectToInterfaceWithString<GpsPrx>( context, objectPrx, proxyString );
            break;
        }
        catch ( const orcaice::TypeMismatchException & e )
        {
            context.tracer()->error( "interface type mismatch. Quitting..." );
            context.communicator()->destroy();
        }
        catch ( const orcaice::NetworkException & e )
        {
            context.tracer()->error( "failed to connect. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: if the user hits ctrl-C, we'll get CommunicatorDestroyedException
        //       which will be caught by Ice::Application and cause exit.
        --count;
    }

    // Get the data once
    try
    {
        cout<<"in special"<<endl;
        context.tracer()->info( "Trying to get one data object as a test" );
        context.tracer()->print( orcaice::toString( objectPrx->getMapGridData() ) );
    }
    catch ( const orca::DataNotExistException & e )
    {
        cout<<e<<endl;
        context.tracer()->warning( "data is not in the buffer yet. Will subscribe anyway." );
    }
    catch ( const orca::HardwareFailedException & e )
    {
        context.tracer()->error( "hardware failure reported when getting data. Will subscribe anyway." );
    }

    // create a callback object to recieve scans
//     Ice::ObjectPtr consumer = new ConsumerIType;
    Ice::ObjectPtr consumer = new orcaice::PrintingConsumerI<GpsMapGridConsumer,GpsMapGridDataPtr>;

    GpsMapGridConsumerPrx callbackPrx = orcaice::createConsumerInterface<GpsMapGridConsumerPrx>( context, consumer );
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // do activation directly on the adapter
    context.adapter()->activate();
    
    //
    // Subscribe for data
    //
    count = 3;
    while ( count )
    {
        try
        {
            objectPrx->subscribeForMapGrid( callbackPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException & e )
        {
            context.tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        --count;
    }
}

// template <> void attach<GpsPrx,GpsTimeConsumerPrx,GpsTimeConsumer,GpsTimeDataPtr>
void attachGpsTime( const orcaice::Context & context, const std::string & proxyString  )
{
    // Connect directly to the interface
    GpsPrx objectPrx;
    int count = 3;
    while( count )
    {
        try
        {
            orcaice::connectToInterfaceWithString<GpsPrx>( context, objectPrx, proxyString );
            break;
        }
        catch ( const orcaice::TypeMismatchException & e )
        {
            context.tracer()->error( "interface type mismatch. Quitting..." );
            context.communicator()->destroy();
        }
        catch ( const orcaice::NetworkException & e )
        {
            context.tracer()->error( "failed to connect. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: if the user hits ctrl-C, we'll get CommunicatorDestroyedException
        //       which will be caught by Ice::Application and cause exit.
        --count;
    }

    // Get the data once
    try
    {
        context.tracer()->info( "Trying to get one data object as a test" );
        context.tracer()->print( orcaice::toString( objectPrx->getTimeData() ) );
    }
    catch ( const orca::DataNotExistException & e )
    {
        cout<<e<<endl;
        context.tracer()->warning( "data is not in the buffer yet. Will subscribe anyway." );
    }
    catch ( const orca::HardwareFailedException & e )
    {
        context.tracer()->warning( "hardware failure reported when getting data. Will subscribe anyway." );
    }

    // create a callback object to recieve scans
//     Ice::ObjectPtr consumer = new ConsumerIType;
    Ice::ObjectPtr consumer = new orcaice::PrintingConsumerI<GpsTimeConsumer,GpsTimeDataPtr>;

    GpsTimeConsumerPrx callbackPrx = orcaice::createConsumerInterface<GpsTimeConsumerPrx>( context, consumer );
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // do activation directly on the adapter
    context.adapter()->activate();
    
    //
    // Subscribe for data
    //
    count = 3;
    while ( count )
    {
        try
        {
            objectPrx->subscribeForTime( callbackPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException & e )
        {
            context.tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        --count;
    }
}

