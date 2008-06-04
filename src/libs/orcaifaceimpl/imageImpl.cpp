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
#include "imageImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class ImageI : public orca::Image
{
public:
    ImageI( ImageImpl &impl )
        : impl_(impl) {}

    // remote interface

    virtual ::orca::ImageDataPtr getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }

    virtual ::orca::ImageDescriptionPtr getDescription(const ::Ice::Current& )
        { return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::ImageConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::ImageConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

private:
    ImageImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

ImageImpl::ImageImpl( const orca::ImageDescriptionPtr& descr,
                      const std::string& interfaceTag,
                      const orcaice::Context& context )
    : descr_(descr),
      interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

ImageImpl::ImageImpl( const orca::ImageDescriptionPtr& descr,
                      const orcaice::Context& context,
                      const std::string& interfaceName )
    : descr_(descr),
      interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

ImageImpl::~ImageImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
ImageImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::ImageConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new ImageI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
ImageImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::ImageConsumerPrx>
        ( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new ImageI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::ImageDataPtr 
ImageImpl::internalGetData() const
{
    context_.tracer().debug( "ImageImpl::internalGetData()", 10 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::ImageDataPtr data;
    dataStore_.get( data );
    return data;
}

::orca::ImageDescriptionPtr
ImageImpl::internalGetDescription() const
{
    return descr_;
}

void 
ImageImpl::internalSubscribe(const ::orca::ImageConsumerPrx& subscriber)
{
    context_.tracer().debug( "ImageImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"ImageImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
ImageImpl::internalUnsubscribe(const ::orca::ImageConsumerPrx& subscriber)
{
    context_.tracer().debug( "ImageImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
ImageImpl::localSet( const orca::ImageDataPtr& data )
{
    dataStore_.set( data );
}

void
ImageImpl::localSetAndSend( const orca::ImageDataPtr& data )
{
//     cout<<"TRACE(ImageIface.cpp): localSetAndSend: " << orcaobj::toString(data) << endl;
    dataStore_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::ImageConsumerPrx,orca::ImageDataPtr>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceName_,
          topicName_ );
}

} // namespace
