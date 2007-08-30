/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "powerreplayer.h"

using namespace std;
using namespace orcalogfactory;

int PowerReplayer::_counter = 0;

PowerReplayer::PowerReplayer( const std::string      &format,
                            const std::string      &filename,
                            const orcaice::Context &context )
    : orcalog::Replayer("Power", format, filename, context)
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }

    stringstream ss;
    ss << "power" << _counter;
    interfaceName_ = ss.str();
    _counter++;
}

PowerReplayer::~PowerReplayer()
{
}

void 
PowerReplayer::initInterfaces()
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, publisher_, interfaceName_ );

    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

orca::PowerData
PowerReplayer::getData(const Ice::Current& current) const
{
    if ( dataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    orca::PowerData data;
    dataPipe_.get( data );

    return data;
}

void 
PowerReplayer::subscribe(const ::orca::PowerConsumerPrx &subscriber, const ::Ice::Current&)
{
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer()->debug( ss.str(), 2 );    
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
PowerReplayer::unsubscribe(const ::orca::PowerConsumerPrx &subscriber, const ::Ice::Current&)
{
    topicPrx_->unsubscribe( subscriber );
}

void 
PowerReplayer::replayData( int index, bool isTest )
{
    checkIndex( index );
    
    if (format_=="ice")
    {
        while ( !file_->eof() && index != (dataCounter_) )
        {
            orcalog::IceReadHelper helper( context_.communicator(), file_ );
            ice_readPowerData( helper.stream_, data_ );
            helper.read();
    
            dataCounter_++;
        }
    }
    else if (format_=="ascii")
    {   
        while ( !file_->eof() && index != (dataCounter_) )
        {
            std::string line;
            std::getline( *file_, line );
//             cout<<"parsing line :"<<line<<endl;
    
            std::stringstream ss( line );
            orcalog::fromLogString( ss, data_ );
    
            dataCounter_++;
        }
    }
    else
    {
        throw orcalog::FormatNotSupportedException( ERROR_INFO, "Unknown format: "+format_ );
    }

    // push to buffer for direct remote access
    dataPipe_.set( data_ );

    if ( !isTest ) 
    {
        // push to IceStorm
        publisher_->setData( data_ );
    }
}
