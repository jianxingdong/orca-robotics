/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/application.h>
#include <orcaice/component.h>
#include <orcaice/orcaice.h>

#include "privateutils.h"

using namespace std;

namespace orcaice {

// argc/argv here are unnecessary.
Application::Application( orcaice::Component &component, bool installCtrlCHandler )
    : IceApplication(installCtrlCHandler),
      component_(component)
{
}

Application::~Application()
{
}

/*
    Order priority for different sources of configuration parameters, from lowest to highest:
        1. orca factory defaults
        2. global config file
        3. component config file
        4. command line arguments
*/
int
Application::main(int argc, char* argv[])
{
    // convert to string sequence for convinience
    Ice::StringSeq args = Ice::argsToStringSeq( argc, argv );

    // parse command line arguments for flags which require immediate action
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,2, "-h" ) ||
             !args[i].compare( 0,6, "--help" ) )
        {
            orcaice::initTracerInfo( component_.help() );
            // nothing to clean up yet
            exit(0);
        }
        else if ( !args[i].compare( 0,2, "-v" ) ||
                  !args[i].compare( 0,9, "--version" ) )
        {
            // print out Ice and libOrcaIce versions.
            orcaice::detail::printVersion();
            // nothing to clean up yet
            exit(0);
        }
    }

    // print version information on the first line
    orcaice::detail::printVersion();

    Ice::InitializationData initData;
    // Note that we don't use the version which takes argumets so that the config file which may be
    // specified by mistake with --Ice.Config is not loaded.
    initData.properties = Ice::createProperties();

    // pre-parse Orca-specific command line arguments
    // (nothing here right now)
    // orcaice::parseOrcaCommandLineOptions( args );

    // Level 4. Highest priority, apply properties from the command line arguments
    // read in all command line options starting with '--", but not "-"
    // note that something like --bullshit will be parsed to --bullshit=1
    // Note that this is a standard Ice function.
    initData.properties->parseCommandLineOptions( "", args );
    initTracerInfo( component_.tag()+": Loaded command line properties" );
            // debug
//             initTracerInfo("after parseCommandLineOptions()");
//             orcaice::detail::printComponentProperties( initData.properties, component_.tag() );

    // Level 3. Now, apply properties from this component's config file (do not force!)
    std::string compFilename;
    try
    {
        compFilename = orcaice::getApplicationConfigFilename( args );
        if ( compFilename.empty() ) {
            initTracerInfo( component_.tag()+": "+warnMissingProperty("component properties file","Orca.Ice") );
        }
        else {
            orcaice::detail::setComponentProperties( initData.properties, compFilename );
            initTracerInfo( component_.tag()+": Loaded component properties from '"+compFilename+"'" );
        }
    }
    catch ( const orcaice::Exception &e )
    {
        initTracerWarning( component_.tag()+": Failed to open component config file '"+compFilename+"':"+e.what() );
            // debug
//         initTracerInfo( component_.tag()+": Application quitting. Orca out." );
//         exit(1);
    }
            // debug
//             initTracerInfo("after setComponentProperties()");
//             orcaice::detail::printComponentProperties( initData.properties, component_.tag() );

    // Level 2. Now, apply properties from the global Orca config file
    std::string globFilename;
    try
    {
        globFilename = orcaice::getGlobalConfigFilename( args );
        orcaice::detail::setGlobalProperties( initData.properties, globFilename );
        initTracerInfo( component_.tag()+": Loaded global properties from '"+globFilename+"'" );
    }
    catch ( const orcaice::Exception &e )
    {
        initTracerWarning( component_.tag()+": Failed to open global config file '"+globFilename+"': "+e.what() );
    }
            // debug
//             initTracerInfo("after setGlobalProperties()");
//             orcaice::detail::printComponentProperties( initData.properties, component_.tag() );

    // Level 1. apply Orca factory defaults
    orcaice::detail::setFactoryProperties( initData.properties, component_.tag() );
    initTracerInfo( component_.tag()+": Loaded factory default properties." );
            // debug
//             initTracerInfo("after setFactoryProperties()");
//             orcaice::detail::printComponentProperties( initData.properties, component_.tag() );

    // now pass the startup options to Ice which will start the Communicator
    return IceApplication::main( argc, argv, initData );
}

int
Application::run( int argc, char* argv[] )
{
    // now communicator exists. we can further parse properties, make sure all the info is
    // there and set some properties (notably AdapterID)
    orca::FQComponentName fqCompName =
                    orcaice::detail::parseComponentProperties( communicator(), component_.tag() );

    // print all prop's now, after some stuff was added, e.g. Tag.AdapterId
    // note: is it possible that some of the prop's got stripped off by Ice::Application::main()? I don't think so.
    if ( communicator()->getProperties()->getPropertyAsInt( "Orca.PrintProperties" ) ) {
        orcaice::detail::printComponentProperties( communicator()->getProperties(), component_.tag() );
    }

    // create the one-and-only component adapter
    adapter_ = communicator()->createObjectAdapter(component_.tag());
    initTracerInfo( component_.tag()+": Object adapter created" );

    //
    // Give the component all the stuff it needs to initialize
    // Cannot change inything in Component::context_ after this step.
    //
    bool isApp = true;
    component_.init( fqCompName, isApp, adapter_ );
    initTracerInfo( component_.tag()+": Application initialized" );

    //
    // Start the component, catching OrcaIce exceptions.
    // Other exception types will get caught by the Ice::Application.
    //
    try
    {
        component_.start();
        // for the components which hug the thread, eg. Qt-based, this will be printed at shutdown
        // this is optional because after the comp is started we can't assume that dumping to cout 
        // will produce the desired result (e.g. if ncurses are used)
        if ( communicator()->getProperties()->getPropertyAsInt( "Orca.PrintComponentStarted" ) ) {
            initTracerInfo( component_.tag()+": Component started" );
        }
    }
    catch ( const orcaice::Exception & e )
    {
        initTracerError( component_.tag()+": Caught OrcaIce exception: "+e.what() );

        // NOTE: in principle, we should try to stop the component first
        //       but, if the exception comes from the constructor, have to make sure we don't try to stop
        //      the threads which were not started yet. Tried in SegwayrRmp and didn't work.
//         initTracerError( component_.tag()+": Unexpected exception from component. Stopping component..." );
//         component_.stop();
//         initTracerInfo( component_.tag()+": Component stopped" );
        
        initTracerInfo( component_.tag()+": Application quitting. Orca out." );
        return 1;
    }

    // component started without a problem. now will wait for Ctrl-C from user or comm
    communicator()->waitForShutdown();
    
    initTracerInfo( component_.tag()+": Communicator is destroyed. Stopping component" );
    component_.finalise();
    component_.stop();
    initTracerInfo( component_.tag()+": Component stopped" );

    adapter_->waitForDeactivate();
    initTracerInfo( component_.tag()+": Adapter deactivated" );

    initTracerInfo( component_.tag()+": Application quitting. Orca out." );

    return 0;
}

} // namespace
