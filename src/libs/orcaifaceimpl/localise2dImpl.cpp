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
#include "localise2dImpl.h"
 

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class Localise2dI : public orca::Localise2d
{
public:
    Localise2dI( Localise2dImpl &impl )
        : impl_(impl) {}

    // remote calls:

    virtual ::orca::Localise2dData getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }
    
    virtual ::orca::VehicleGeometryDescriptionPtr getVehicleGeometry( const ::Ice::Current& )
        { return impl_.internalGetVehicleGeometry(); }    

    virtual void subscribe(const ::orca::Localise2dConsumerPrx& subscriber,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( subscriber ); }

    virtual void unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( subscriber ); }

private:
    Localise2dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

Localise2dImpl::Localise2dImpl( const orca::VehicleGeometryDescriptionPtr &geometry,
                                const std::string &interfaceTag,
                                const orcaice::Context &context,
                                TopicPolicy policy )
    : geometry_(geometry),
      policy_(policy),
      interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
      topicName_(orcaice::toTopicAsString(context.name(),interfaceName_)),
      context_(context)
{
    assert( geometry_ != 0 );
}

Localise2dImpl::Localise2dImpl( const orca::VehicleGeometryDescriptionPtr &geometry,
                                const orcaice::Context &context,
                                const std::string &interfaceName,
                                TopicPolicy policy )
    : geometry_(geometry),
      policy_(policy),
      interfaceName_(interfaceName),
      topicName_(orcaice::toTopicAsString(context.name(),interfaceName_)),
      context_(context)
{
    assert( geometry_ != 0 );
}

Localise2dImpl::~Localise2dImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );

    if ( policy_ == DestroyTopic ) {
    //     tryDestroyTopic( topicPrx_ );
        if ( !context_.communicator() ) {
            // the communicator is already destroyed.
            return;
        }
    
        try {
            topicPrx_->destroy();
            context_.tracer().info( "Destroyed IceStorm topic "+topicName_ );
        }
        catch ( std::exception &e )
        {
            stringstream ss;
            ss << "(while destroying IceStorm topic) caught exception: " << e.what();
            context_.tracer().warning( ss.str() );
        }
    }
}

void
Localise2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::Localise2dConsumerPrx>
        ( context_, publisherPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new Localise2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
Localise2dImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::Localise2dConsumerPrx>
        ( context_, publisherPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new Localise2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::Localise2dData 
Localise2dImpl::internalGetData() const
{
    context_.tracer().debug( "Localise2dImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Localise2dData data;
    dataStore_.get( data );
    return data;
}

::orca::VehicleGeometryDescriptionPtr
Localise2dImpl::internalGetVehicleGeometry() const
{
    return geometry_;
}

void 
Localise2dImpl::internalSubscribe(const ::orca::Localise2dConsumerPrx& subscriber)
{
    context_.tracer().debug( "Localise2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"Localise2dImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Localise2dImpl::internalUnsubscribe(const ::orca::Localise2dConsumerPrx& subscriber)
{
    context_.tracer().debug( "Localise2dImpl::internalUnsubscribe(): unsubscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Localise2dImpl::localSet( const orca::Localise2dData &data )
{
    dataStore_.set( data );
}

void
Localise2dImpl::localSetAndSend( const orca::Localise2dData &data )
{
    dataStore_.set( data );
    
    // Try to push to IceStorm.
    orcaice::tryPushToIceStormWithReconnect<orca::Localise2dConsumerPrx,orca::Localise2dData>
        ( context_,
          publisherPrx_,
          data,
          topicPrx_,
          topicName_ );
}

} // namespace
