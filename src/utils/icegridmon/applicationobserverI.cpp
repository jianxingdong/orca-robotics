#include "applicationobserverI.h"
#include <iostream>
#include "printutil.h"

using namespace std;

namespace icegridmon {

void 
ApplicationObserverI::applicationInit( int serial,
                                       const IceGrid::ApplicationInfoSeq &applications,
                                       const Ice::Current& )
{
    stringstream ss;
    ss << "ApplicationObserverI::applicationInit:\n"
       << "  serial: " << serial << endl;
    ss << "  applications: " << endl << toString(applications,"application",4);
    context_.tracer()->info( ss.str() );
}

void 
ApplicationObserverI::applicationAdded( int serial, 
                                        const IceGrid::ApplicationInfo &desc,
                                        const Ice::Current& )
{
    stringstream ss;
    ss << "ApplicationObserverI::applicationAdded:\n"
       << "  serial: " << serial << endl
       << toString(desc,2);
    context_.tracer()->info( ss.str() );
}

void 
ApplicationObserverI::applicationRemoved( int serial, 
                                          const std::string &name,
                                          const Ice::Current& )
{
    stringstream ss;
    ss << "ApplicationObserverI::applicationRemoved:\n"
       << "  serial: " << serial << endl
       << "  name:   " << name;
    context_.tracer()->info( ss.str() );
}

void 
ApplicationObserverI::applicationUpdated( int serial,
                                          const IceGrid::ApplicationUpdateInfo &desc,
                                          const Ice::Current& )
{
    stringstream ss;
    ss << "ApplicationObserverI::applicationUpdated:\n"
       << "  serial: " << serial << endl
       << toString(desc,2);
    context_.tracer()->info( ss.str() );
}

}