/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "imulogger.h"

using namespace std;

namespace orcalogfactory {

ImuLogger::ImuLogger( orcalog::LogMaster *master, 
                              const std::string & typeSuffix,
                              const std::string & format,
                              const std::string & filenamePrefix,
                              const orcaice::Context & context )
    : orcalog::Logger( master, 
                      "::orca::Imu",
                      "Imu"+typeSuffix,
                      format,
                      filenamePrefix+"imu"+typeSuffix+".log",
                      context )
{   
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

void 
ImuLogger::init()
{      
    // throws orcaice::NetworkException but it's ok to quit here
    orca::ImuPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::ImuPrx>( context_, objectPrx, interfaceTag_ );
    
    // get description. should do try/catch
    orca::ImuDescription obj = objectPrx->getDescription();
    writeDescription( obj );
    
    // consumer
    Ice::ObjectPtr consumer = this;
    orca::ImuConsumerPrx callbackPrx = 
        orcaice::createConsumerInterface<orca::ImuConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}

void 
ImuLogger::setData(const orca::ImuData& data, const Ice::Current&)
{
    // Write a reference in the master file
    appendMasterFile();
    
    if ( format_=="ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeImuData( helper.stream_, data );
        helper.write( file_ );
    }
    else if ( format_=="ascii" )
    {
	   (*file_) << orcalog::toLogString(data) << endl;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}


void 
ImuLogger::writeDescription( const orca::ImuDescription& obj )
{
    context_.tracer()->debug( "Writing description to file", 5 );
    
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeImuDescription( helper.stream_, obj );
        helper.write( file_ );  
    }
    else if ( format_ == "ascii" )
    {
       context_.tracer()->warning( "Writing description to file in ascii not implemented" );
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

}

