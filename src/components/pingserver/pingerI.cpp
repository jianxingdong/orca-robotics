/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "pingerI.h"
#include <iostream>
#include <orcaping/orcaping.h>
#include <orcaice/orcaice.h>

using namespace std;

namespace pingserver {

PingerI::PingerI( const std::string      &ifaceTag,
                  const orcaice::Context &context )
    : pinger_(new orcaping::Pinger),
      ifaceTag_(ifaceTag),
      context_(context)
{
    // Experimental ping to make sure everything's OK:
    try {
        pinger_->ping( "localhost" );
    }
    catch ( orcaping::PingException &e )
    {
        stringstream ss;
        ss << "Experimental ping to localhost failed: " << e.what();
        context_.tracer()->error( ss.str() );
        throw orcaping::PingException( ss.str() );
    }
}

PingerI::~PingerI()
{
    delete pinger_;
}

void
PingerI::initInterface()
{
    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, ifaceTag_ );
}

double 
PingerI::ping( const std::string &host, const Ice::Current& )
{
    stringstream ss; ss << "Pinging "<<host;
    context_.tracer()->debug( ss.str() );

    try {
        double latencyMs = pinger_->ping( host );
        stringstream ss; ss << "  Ping latency: " << latencyMs << "ms";
        context_.tracer()->debug( ss.str() );
        return latencyMs;
    }
    catch ( orcaping::PingException &e )
    {
        stringstream ss; ss << "Error while pinging: " << e.what();
        context_.tracer()->debug( ss.str() );
        throw orca::PingException( e.what() );
    }
}

}