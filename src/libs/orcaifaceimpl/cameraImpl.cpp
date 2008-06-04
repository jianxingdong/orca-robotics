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

#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "cameraImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class CameraI : public orca::Camera
{
public:
    CameraI( CameraImpl &impl )
        : impl_(impl) {}

    // remote interface

    virtual ::orca::CameraDataSequence getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }

    virtual ::orca::CameraDescriptionSequence getDescription(const ::Ice::Current& ) const
        { return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::CameraConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::CameraConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

private:
    CameraImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

CameraImpl::CameraImpl( const orca::CameraDescriptionSequence& descr,
                        const std::string& interfaceTag,
                        const orcaice::Context& context )
    : descr_(descr),
      interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

CameraImpl::CameraImpl( const orca::CameraDescriptionSequence& descr,
                        const orcaice::Context& context,
                        const std::string& interfaceName )
    : descr_(descr),
      interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

CameraImpl::~CameraImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
CameraImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::CameraConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new CameraI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
CameraImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::CameraConsumerPrx>
        ( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new CameraI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::CameraDataSequence 
CameraImpl::internalGetData() const
{
    context_.tracer().debug( "CameraImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::CameraDataSequence data;
    dataStore_.get( data );
    return data;
}

::orca::CameraDescriptionSequence
CameraImpl::internalGetDescription() const
{
    return descr_;
}

void 
CameraImpl::internalSubscribe(const ::orca::CameraConsumerPrx& subscriber)
{
    context_.tracer().debug( "CameraImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer().debug( ss.str(), 2 );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"CameraImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
CameraImpl::internalUnsubscribe(const ::orca::CameraConsumerPrx& subscriber)
{
    context_.tracer().debug( "CameraImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
CameraImpl::localSet( const orca::CameraDataSequence& data )
{
    dataStore_.set( data );
}

void
CameraImpl::localSetAndSend( const orca::CameraDataSequence& data )
{
//     cout<<"TRACE(CameraIface.cpp): localSetAndSend: " << orcaobj::toString(data) << endl;

    dataStore_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::CameraConsumerPrx,orca::CameraDataSequence>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceName_,
          topicName_ );
}

} // namespace
