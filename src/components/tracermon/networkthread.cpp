/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "networkthread.h"
#include "events.h"
#include "tracerconsumerI.h"

using namespace std;
using namespace tracermon;

MainThread::MainThread( User* user, const orcaice::Context & context ) :
    SafeThread( context.tracer() ),
    user_(user),
    events_(new hydroutil::EventQueue),
    context_(context)
{
}

MainThread::~MainThread()
{
    user_->newLocalTrace( "MainThread distructor" );
}

void 
MainThread::setVerbosityLevel( int error, int warn, int info, int debug )
{
// stringstream ss;
// ss<<"got verbosity event with ["<<error<<","<<warn<<","<<info<<","<<debug<<"]";
// user_->newLocalTrace( ss.str() );
// user_->newLocalTrace( "got fucking verbosity event" );
    hydroutil::EventPtr e = new VerbosityLevelsChangedEvent( error, warn, info, debug );
    events_->add( e );
}

void
MainThread::walk()
{    
    // configs
    std::string prefix = context_.tag() + ".Config.";
    Ice::PropertiesPtr props = context_.properties();
    int infoVerb = orcaice::getPropertyAsIntWithDefault( props, prefix+"InfoVerbosity", 10 );
    int warnVerb = orcaice::getPropertyAsIntWithDefault( props, prefix+"WarningVerbosity", 10 );
    int errorVerb = orcaice::getPropertyAsIntWithDefault( props, prefix+"ErrorVerbosity", 10 );
    int debugVerb = orcaice::getPropertyAsIntWithDefault( props, prefix+"DebugVerbosity", 10 );

    // multi-try
    orcaice::activate( context_, this ); 

    // REQUIRED : Tracer
    // multi-try
    orcaice::connectToInterfaceWithTag<orca::TracerPrx >( context_, tracerPrx_, "Tracer", this );

    //
    // this loop until it succeeds
    //
// stringstream ss;
// ss<<"init verbosity with ["<<errorVerb<<","<<warnVerb<<","<<infoVerb<<","<<debugVerb<<"]";
// user_->newLocalTrace( ss.str() );
    setRemoteVerbosity( errorVerb, warnVerb, infoVerb, debugVerb );

    //
    // Subscribe for data
    //
//     Ice::ObjectPtr consumer = this;
    Ice::ObjectPtr consumer = new TracerConsumerI( *user_ );
    orca::TracerConsumerPrx callbackPrx = 
            orcaice::createConsumerInterface<orca::TracerConsumerPrx>( context_, consumer );


// user_->newLocalTrace( "subscribing..." );
    while ( !isStopping() ) {
        try
        {
//             tracerPrx_->setVerbosity( errorVerb, warnVerb, infoVerb, debugVerb );
//             user_->newVerbosityLevel( errorVerb, warnVerb, infoVerb, debugVerb );
            tracerPrx_->subscribeForComponentMessages( callbackPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException& e )
        {
            context_.tracer().warning( "remote exception orca::SubscriptionFailedException: "+e.what );
            context_.tracer().warning( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        catch ( const Ice::Exception& )
        {
            context_.tracer().error( "Ice exception. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }

    hydroutil::EventPtr event;
    int timeoutMs = 500;

    //
    // Main loop
    //
    while ( !isStopping() )
    {
        if ( !events_->timedGet( event, timeoutMs ) ) {
            continue;
        }

        switch ( event->type() )
        {
        // approx in order of call frequency
        case VerbosityLevelsChanged : {
            VerbosityLevelsChangedEventPtr e = VerbosityLevelsChangedEventPtr::dynamicCast( event );
            setRemoteVerbosity( e->error_, e->warn_, e->info_, e->debug_ );
            break;
        }
        default : {
            stringstream ss; ss<<"unknown network event "<<event->type()<<". Ignoring...";
            user_->newLocalTrace( "MainThread: entering main loop" );
        }
        } // switch
    } // end of main loop

    context_.tracer().debug( "MainThread: stopped.",2 );
}

void 
MainThread::setRemoteVerbosity( int error, int warn, int info, int debug )
{
    stringstream ss; ss<<"trying to set verbosity with ["<<error<<","<<warn<<","<<info<<","<<debug<<"]";
    user_->newLocalTrace( ss.str() );

    orca::TracerVerbosityConfig config;
    config.error = error;
    config.warning = warn;
    config.info = info;
    config.debug = debug;

    while ( !isStopping() ) {
        try
        {
            tracerPrx_->setVerbosity( config );

            // give feedback to the user
            config = tracerPrx_->getVerbosity();
            user_->newVerbosityLevel( config.error, config.warning, config.info, config.debug );
            break;
        }
        catch ( const orca::SubscriptionFailedException& e )
        {
            context_.tracer().warning( "remote exception orca::SubscriptionFailedException: "+e.what );
            context_.tracer().warning( "failed to subscribe for data updates. Will try again after 1 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        catch ( const Ice::Exception& )
        {
            context_.tracer().error( "Ice exception. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
}

// void 
// MainThread::setData(const orca::TracerData& data, const Ice::Current&)
// {
//     cout<<"MainThread::setData "<<endl;
//     user_->newTraceMessage( data );
// }
