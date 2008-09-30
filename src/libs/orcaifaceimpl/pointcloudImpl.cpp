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
#include "pointcloudImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class PointCloudI : public orca::PointCloud
{
public:
    //! constructor
    PointCloudI( PointCloudImpl &impl )
        : impl_(impl) {}
    virtual ~PointCloudI() {}

    // remote interface

    virtual ::orca::PointCloudData getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }

    //virtual ::orca::PointCloudDescription getDescription(const ::Ice::Current& ) const
        //{ return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::PointCloudConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::PointCloudConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe(consumer); }

private:
    PointCloudImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

PointCloudImpl::PointCloudImpl( //const orca::PointCloudDescription &descr,
                                const std::string              &interfaceTag,
                                const orcaice::Context         &context )
    : //descr_(descr),
      interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

PointCloudImpl::PointCloudImpl( //const orca::PointCloudDescription &descr,
                                const orcaice::Context         &context,
                                const std::string              &interfaceName )
    : //descr_(descr),
      interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

PointCloudImpl::~PointCloudImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
PointCloudImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::PointCloudConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new PointCloudI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
PointCloudImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::PointCloudConsumerPrx>
        ( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new PointCloudI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::PointCloudData 
PointCloudImpl::internalGetData() const
{
    context_.tracer().debug( "PointCloudImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::PointCloudData data;
    dataStore_.get( data );
    return data;
}

//::orca::PointCloudDescription
//PointCloudImpl::internalGetDescription() const
//{
    //return descr_;
//}

void 
PointCloudImpl::internalSubscribe(const ::orca::PointCloudConsumerPrx& subscriber )
{
    context_.tracer().debug( "PointCloudImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"PointCloudImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
PointCloudImpl::internalUnsubscribe(const ::orca::PointCloudConsumerPrx& subscriber)
{
    context_.tracer().debug( "PointCloudImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
PointCloudImpl::localSet( const orca::PointCloudData& data )
{
    dataStore_.set( data );
}

void
PointCloudImpl::localSetAndSend( const orca::PointCloudData& data )
{
    dataStore_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::PointCloudConsumerPrx,orca::PointCloudData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceName_,
          topicName_ );
}

} // namespace