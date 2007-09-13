/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "pixmapImpl.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class PixMapI : public orca::PixMap
{
public:

    PixMapI( PixMapImpl &impl )
        : impl_(impl) {}

    // Remote calls:
    orca::PixMapData getData(const Ice::Current&) const
        { return impl_.internalGetData(); }

    virtual void subscribe(const ::orca::PixMapConsumerPrx& consumer,
                           const Ice::Current&)
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::PixMapConsumerPrx& consumer,
                             const Ice::Current&)
        { impl_.internalUnsubscribe( consumer ); }

private:
    PixMapImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

PixMapImpl::PixMapImpl( const std::string      &interfaceTag,
                  const orcaice::Context &context ) 
    : interfaceTag_(interfaceTag),
      context_(context)      
{
}

PixMapImpl::~PixMapImpl()
{
    tryRemovePtr( context_, ptr_ );
}

void
PixMapImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<PixMapConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new PixMapI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_ );
}

PixMapData
PixMapImpl::internalGetData() const
{
    context_.tracer()->debug( "PixMapImpl::internalGetData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interfaceTag="<<interfaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    PixMapData data;
    dataProxy_.get( data );
    return data;
}

void
PixMapImpl::internalSubscribe(const ::PixMapConsumerPrx& subscriber )
{
    context_.tracer()->debug( "PixMapImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"PixMapImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
PixMapImpl::internalUnsubscribe(const ::PixMapConsumerPrx& subscriber )
{
    context_.tracer()->debug( "PixMapImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void 
PixMapImpl::localSetAndSend( const ::orca::PixMapData &data )
{
    //cout<<"TRACE(ogmapI.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<PixMapConsumerPrx,orca::PixMapData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceTag_ );
}

}