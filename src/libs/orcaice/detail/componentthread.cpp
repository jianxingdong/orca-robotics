#include "componentthread.h"
#include <orcaice/exceptions.h>
#include <iostream>
#ifdef ICE_32
#include <IceGrid/Registry.h>  // used to register Home interface as a well-known object
#else
#include <IceGrid/Admin.h>  // used to register Home interface as a well-known object
#endif

using namespace std;

namespace orcaice {

ComponentThread::ComponentThread( const Ice::ObjectPrx   &homePrx,
                                  Status                 &status,
                                  ComponentInterfaceFlag  interfaceFlag,
                                  const orcaice::Context &context )
    : registeredHome_(false),
      homePrx_(homePrx),
      status_(status),
      interfaceFlag_(interfaceFlag),
      context_(context)
{
}

ComponentThread::~ComponentThread()
{
}

void
ComponentThread::run()
{
    bool hasStatusInterface = (interfaceFlag_ & StatusInterface);
    bool hasHomeInterface   = (interfaceFlag_ & HomeInterface);

    try {
        while ( isActive() )
        {
            bool needToRegisterHome = hasHomeInterface && !registeredHome_;
            if ( !needToRegisterHome && !hasStatusInterface )
            {
                // Nothing left for us to do!
                stringstream ss;
                ss << "ComponentThread: Nothing left to do, so quitting. hasStatusInterface: " << hasStatusInterface;
                context_.tracer()->debug( ss.str() );
                return;
            }

            if ( !registeredHome_ && (interfaceFlag_ & HomeInterface) )
            {
                registeredHome_ = tryRegisterHome();
            }
            if ( interfaceFlag_ & StatusInterface )
                status_.process();

            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        }
    }
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK, we must be shutting down.
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "ComponentThread: caught unexpected exception: " << e.what() <<".  This shouldn't happen.";
        context_.tracer()->error( ss.str() );
    }
}

bool
ComponentThread::tryRegisterHome()
{
#ifdef ICE_32
    //
    // PROVIDED INTERFACE: Home
    // Make Home a well-known object, by adding it to the registry
    //

    std::string instanceName = context_.properties()->getPropertyWithDefault( "IceGrid.InstanceName", "IceGrid" );
    Ice::ObjectPrx base = context_.communicator()->stringToProxy( instanceName+"/Registry" );
    try {
        // Open an admin session with the registry
        IceGrid::RegistryPrx registry = IceGrid::RegistryPrx::checkedCast(base);
        // This assumes no access control
        std::string username = "no-access-control-assumed";
        std::string password = "no-access-control-assumed";
        IceGrid::AdminSessionPrx adminSession = registry->createAdminSession( username, password );

        // use the adminSession to add our Home interface
        IceGrid::AdminPrx admin = adminSession->getAdmin();
        try {
            admin->addObjectWithType( homePrx_, "::orca::Home" );
        }
        catch ( const IceGrid::ObjectExistsException& ) {
            admin->updateObject( homePrx_ );
        }
    }
    catch ( Ice::Exception& e ) 
    {
        bool requireRegistry = context_.properties()->getPropertyAsInt( "Orca.RequireRegistry" );
        if ( requireRegistry ) {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".  Check IceGrid Registry.  You may allow things to continue without registration by setting Orca.RequireRegistry=0.";
            context_.tracer()->error( ss.str() );
            return false;
        }
        else {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".";
            context_.tracer()->warning( ss.str() );
            context_.tracer()->info( "You may enforce registration by setting Orca.RequireRegistry=1." );
            return true;
        }
    }
#else
    //
    // PROVIDED INTERFACE: Home
    // Make Home a well-known object, by adding it to the registry
    //
    std::string instanceName = properties()->getPropertyWithDefault( "IceGrid.InstanceName", "IceGrid" );
    Ice::ObjectPrx adminPrx = context_.communicator()->stringToProxy( instanceName+"/Admin" );
    
    IceGrid::AdminPrx admin;
    try {
        admin = IceGrid::AdminPrx::checkedCast( adminPrx );
        admin->addObjectWithType( homePrx_, "::orca::Home" );
    } 
    catch (const IceGrid::ObjectExistsException&) {
        admin->updateObject( homePrx_ );
    }
    catch ( Ice::Exception& e ) {
        bool requireRegistry = context_.properties()->getPropertyAsInt( "Orca.RequireRegistry" );
        if ( requireRegistry ) {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".  Check IceGrid Registry.  You may allow things to continue without registration by setting Orca.RequireRegistry=0.";
            context_.tracer()->error( ss.str() );
            return false;
        }
        else {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".";
            context_.tracer()->warning( ss.str() );
            context_.tracer()->info( "You may enforce registration by setting Orca.RequireRegistry=1." );
            return true;
        }
    }
    return (Home*)hobj;
#endif
    return true;
}


}