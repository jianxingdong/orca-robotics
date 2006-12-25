/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_CONNECT_UTILITIES_H
#define ORCAICE_CONNECT_UTILITIES_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

#include <orcaice/context.h>
#include <orcaice/exceptions.h>
#include <orcaice/configutils.h>
#include <orcaice/sysutils.h>
#include <orcaice/printutils.h>

#include <orcaobj/stringutils.h>

namespace orcaice
{
/*!
 *  @name Connection Functions
 */
//@{

/*!
 *  Tries to ping the default Locator and, based on success or the exceptions
 *  it catches, determines if the network is up and if the registry is reachable.
 */
void getNetworkState( const Context  & context,
                      bool           & isNetworkUp, 
                      bool           & isRegistryUp );

/*!
 *  Adds the @p object to the component adapter and gives it the @p name.
 *  Note that @p name is just the interface name, not its fully-qualified name.
 *  (In Ice terms this will become the object identity.)
 *
 *  Throws orcaice::Exception if fails to register the new object.
 */
void createInterfaceWithString( const Context       & context,
                                Ice::ObjectPtr      & object,
                                const std::string   & name );
                            
/*!
 * Convenience function, behaves like @ref createInterfaceWithString but the proxy information
 * is looked up in the @p context properties based on the @p interfaceTag.
 *
 * Throws ConfigFileException if the interface name cannot be read from the config file for some reason.
 */
void createInterfaceWithTag( const Context      & context,
                            Ice::ObjectPtr      & object,
                            const std::string   & interfaceTag );

/*!
 * Adds an object to the adapter and returns a direct proxy to the object. A
 * globally unique name is supplied. This function is useful for creating
 * 'consumer' objects, i.e. you subscribe for information to be pushed into
 * them by specifying the direct proxy to them.
 *
 * This is a local operation which does not throw any exceptions.
 */
template<class ConsumerProxyType>
ConsumerProxyType
createConsumerInterface( const Context  & context,
                         Ice::ObjectPtr & consumer )
{
    // create servant and tell adapter about it (let it make up a globally unique name)
    // NOTE: addWithUUID() does not throw exceptions.
    Ice::ObjectPrx obj = context.adapter()->addWithUUID( consumer );
    
    // make a direct proxy
    Ice::ObjectPrx prx = context.adapter()->createDirectProxy( obj->ice_getIdentity() );
    return ConsumerProxyType::uncheckedCast( prx );
}

//
//  Implementation Note:
//  the two 'connectToInterface' functions have to be templated, unfortunately,
//  to ensure we're connecting to the right type.
/*!
 *  Create an Ice proxy @p proxy for the remote server based on a stringified proxy @p proxyString.
 *  The proxy can be direct or indirect.
 *
 *  Throws TypeMismatchException if fails to connect to the remote interface or if
 *  it is of the wrong type. Throws NetworkException if the interface is otherwise unreachable.
 *
 *  Example: to connect to interface of type MyInterface, use
@verbatim
MyInterfacePrx myInterfacePrx;
orcaice::connectToInterfaceWithTag<MyInterfacePrx>( context(), myInterfacePrx, "iface@platform/component" );
@endverbatim
 */
template<class ProxyType>
void
connectToInterfaceWithString( const Context     & context,
                              ProxyType         & proxy,
                              const std::string & proxyString)
{
    Ice::ObjectPrx base = context.communicator()->stringToProxy(proxyString);

    // check with the server that the one we found is of the right type
    // the check operation is remote and may fail (see sec.6.11.2)
    try {
        proxy = ProxyType::checkedCast( base );
        // got some answer, check that it's the right type
        if ( !proxy ) {
            std::string errString = "Required interface '" + proxyString + "' is of wrong type.";
            initTracerWarning( context, errString, 2 );
            throw orcaice::TypeMismatchException( ERROR_INFO, errString );
        }
    }
    // typically we get ConnectionRefusedException, but it may be ObjectNotExistException
    catch ( Ice::LocalException &e )
    {
        // Give some feedback as to why shit isn't working
        std::stringstream ss;
        ss << "Failed to connect to '" << proxyString << "': "<<e;
        initTracerWarning( context, ss.str(), 2 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }

    //! @todo Record who we connected to as a required interface so that Home can report this.
}

/*!
 *  Convenience function, behaves like @ref connectToInterfaceWithString but the proxy information
 *  comes from the configuration file and the @p interfaceTag.
 *
 *  Throws ConfigFileException if the interface name cannot be read for some reason.
 */
template<class ProxyType>
void
connectToInterfaceWithTag( const Context       & context,
                           ProxyType           & proxy,
                           const std::string   & interfaceTag )
{
    // this may throw ConfigFileException, we don't catch it, let the user catch it at the component level
    std::string proxyString = orcaice::getRequiredInterfaceAsString( context, interfaceTag );

    // now that we have the stingified proxy, use the function above.
    connectToInterfaceWithString( context, proxy, proxyString );
}

// FUNCTIONS WITHOUT DOXYGEN TAGS ARE UTILITY FUNCTIONS
// THEY ARE PUBLICLY AVAILABLE BUT ARE NOT ADVERTIZED THROUGH DOXYGEN

/*
 * Behaves the same as the one above but connects to the topic manager
 * specified in the current properties.
 */
IceStorm::TopicPrx connectToIceStormTopicPrx( const Context &,
                                              const std::string & topicName,
                                              bool createIfMissing=false );

/*
 * Behaves the same as the one above but connects to the topic manager
 * specified in the communicator's properties.
 */
IceStorm::TopicPrx connectToIceStormTopicPrx( const Ice::CommunicatorPtr & communicator,
                                              const std::string & topicName,
                                              bool createIfMissing=false );

/*
 * Given the stingified proxy to topic manager and topic name, connect to the topic proxy.
 * If topic manager does not exist, throws Exception exception.
 */
IceStorm::TopicPrx connectToIceStormTopicPrxWithManager( const Ice::CommunicatorPtr & communicator,
                                    const std::string & topicName,
                                    const std::string & topicManagerString,
                                    bool createIfMissing=false );

/*
 * Publisher is used from the provider end. It is the consumer of information.
 * So you can push data into it.
 */
Ice::ObjectPrx connectToIceStormTopicPublisherPrx( const Ice::CommunicatorPtr & communicator,
                                    const std::string & topicName );

/*
 * Behaves like the one above.
 */
Ice::ObjectPrx connectToIceStormTopicPublisherPrx( const IceStorm::TopicPrx & topic );


/*!
 *  Convenience function, behaves like @ref connectToTopicWithString but the proxy information
 *  comes from the configuration file and the @p interfaceTag. 
 *  The appropriate topic name is generated based on the fully-qualified interface name. Default
 *  subtopic is "*".
 *
 *  Throws ConfigFileException if the interface name cannot be read for some reason.
 */
template<class ConsumerProxyType>
IceStorm::TopicPrx
connectToTopicWithTag( const Context           & context,
                       ConsumerProxyType       & publisher,
                       const std::string       & interfaceTag,
                       const std::string       & subtopic="*" )
{
    std::string topicName;

    // lookup the name of the interface in the config file and generate topic name.
    try {
        // this generates a standard topic name based on fully-qualified interface name.
        topicName = orcaice::toString(
                        orcaice::getProvidedTopic( context, interfaceTag, subtopic ) );
    }
    catch ( orcaice::Exception &e )
    {
        std::string errString = e.what();
        throw orcaice::ConfigFileException( ERROR_INFO,
                "Couldn't get name for IceStorm topic tag '"+interfaceTag+"': "+errString );
    }

    return connectToTopicWithString( context, publisher, topicName );
}

/*!
 *  This function is used by potential publishers of information. It returns
 *  a proxy to the IceStorm topic and sets proxy to the topic's @p publisher.
 *  The publisher is used to distribute information to consumers and the
 *  topic is used to subscribe/unsubscribe data consumers.
 *
 *  The address of IceStorm server is looked up in the @c IceStorm.TopicManager.Proxy property.
 *
 *  Raises NetworkException if the server cannot be reached or Exception if there is any other
 *  problem.
 */
template<class ConsumerProxyType>
IceStorm::TopicPrx
connectToTopicWithString( const Context     & context,
                          ConsumerProxyType & publisher,
                          const std::string & topicName )
{
    IceStorm::TopicPrx topicPrx;

    try {
        const bool createIfMissing = true;
        //
        // set the proxy to the topic
        //
        topicPrx = connectToIceStormTopicPrx( context.communicator(), topicName, createIfMissing );

        Ice::ObjectPrx obj = connectToIceStormTopicPublisherPrx( topicPrx );
        //
        // set the proxy to the publisher
        //
        publisher = ConsumerProxyType::uncheckedCast(obj);
    }
    //catch ( const orcaice::Exception & e ) {
        // we'll catch it here if the topic manager does not exist
    //}
    catch ( Ice::ConnectionRefusedException &e )
    {
        // Give some feedback as to why this isn't working
        std::stringstream ss; ss<<"Error while connecting to IceStorm topic publisher '"<<topicName<<"': "<<e;
        initTracerError( context, ss.str(), 2 );
        initTracerPrint( context, "hint: Is IceStorm running?", 5 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
    catch( const Ice::LocalException &e )
    {
        std::stringstream ss;
        ss<<"Error while connecting to IceStorm topic publisher '"<<topicName<<"': "<<e;
        initTracerError( context, ss.str(), 2 );
        throw orcaice::Exception( ERROR_INFO, ss.str() );
    }
    catch ( Ice::Exception &e )
    {
        // Give some feedback as to why this isn't working
        std::stringstream ss; ss<<"Error while connecting to IceStorm topic publisher '"<<topicName<<"': "<<e;
        initTracerError( context, ss.str(), 2 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }

    return topicPrx;
}

// THESE FUNCTIONS WERE OBSOLETED TO REDUCE THE SIZE OF THE API
/*
 * Subscribe to an IceStorm topic, when you know its name.
 * Utility function, use with care. Standard components should subscribe
 * themselves by talking directly to provided interfaces.
 */
// void
// subscribeToIceStormTopicWithName( const Context         & context,
//                                     Ice::ObjectPtr      & consumer,
//                                     const std::string   & topicName,
//                                     bool                createIfMissing=false );

/*
 * Behaves the same as the one above but the topic information
 * comes from the configuration file and the interfaceTag.
 */
// void
// subscribeToIceStormTopicWithTag( const Context      & context,
//                                 Ice::ObjectPtr      & consumer,
//                                 const std::string   & interfaceTag,
//                                 bool                createIfMissing=false );

//@}
} // namespace

#endif
