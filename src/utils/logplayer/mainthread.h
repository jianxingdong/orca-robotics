/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <string>
#include <memory>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>

#include <hydrodll/dll.h>
#include "interfacetypecounter.h"

namespace orcalog
{
    class MasterFileReader;
    class Replayer;
    class ReplayerFactory;
}

namespace logplayer
{

class MainThread: public orcaice::SubsystemThread
{    	
public:
    MainThread( const orcaice::Context& context );
    ~MainThread();

private:
    // from SubsystemThread
    virtual void initialise();
    // this subsystem does not work!

    // utilities
    void loadPluginLibraries( const std::string& factoryLibNames );
    void createReplayer( const std::string & interfaceType, 
                         const std::string& format,
                         const std::string& filename,
                         bool               enabled,
                         bool               require );

    void readReplayParams( IceUtil::Time &beginTime, double &replayRate, bool &autoStart );

    gbxiceutilacfr::ThreadPtr       replayConductor_;
    gbxiceutilacfr::ThreadPtr       highLevelController_;

    std::auto_ptr<orcalog::MasterFileReader> masterFileReader_;

    std::vector<orcalog::ReplayerFactory*>           replayerFactories_;
    std::vector<hydrodll::DynamicallyLoadedLibrary*> libraries_;

    std::vector<orcalog::Replayer*> replayers_;

    // Ensure interfaces of a given type are unique.
    InterfaceTypeCounter interfaceTypeCounter_;
    
    orcaice::Context context_;
};

} // namespace

#endif
