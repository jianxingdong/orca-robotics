/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "powerI.h"

using namespace std;
using namespace orca;
using namespace robot2d;

PowerI::PowerI( orcaice::PtrProxy<orca::PowerDataPtr> &power, const IceStorm::TopicPrx &topic )
      : powerPipe_(power),
        topic_(topic)
{
}

orca::PowerDataPtr
PowerI::getData(const ::Ice::Current& ) const
{
    //std::cout << "Sending data back" << std::endl;

    // create null smart pointer. data will be cloned into it.
    orca::PowerDataPtr data;

    try
    {
        powerPipe_.get( data );
    }
    catch ( const orcaice::Exception & e )
    {
        throw orca::DataNotExistException( "power proxy is not populated yet" );
    }
    return data;
}

void
PowerI::subscribe(const ::orca::PowerConsumerPrx& subscriber, const ::Ice::Current&)
{
    //cout<<"subscription request"<<endl;
    IceStorm::QoS qos;
    topic_->subscribe( qos, subscriber );
}

void
PowerI::unsubscribe(const ::orca::PowerConsumerPrx& subscriber, const ::Ice::Current&)
{
    //cout<<"unsubscription request"<<endl;
    topic_->unsubscribe( subscriber );
}
