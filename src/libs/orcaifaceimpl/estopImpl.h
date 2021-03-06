#ifndef ORCA_ESTOP_IMPL_H
#define ORCA_ESTOP_IMPL_H

#include <memory>
#include <orca/estop.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>
#include <orcaice/topichandler.h>

namespace gbxutilacfr { class Stoppable; }

namespace orcaifaceimpl
{

//!
//! @brief EStop needs a pointer to a callback object implementing these.
//!
class AbstractEStopCallback {
public:
    virtual ~AbstractEStopCallback() {}

    virtual void setEnabled( bool enabled )=0;
};

//!
//! Implements the EStopIface interface: Handles remote calls.
//!
class EStopImpl : public IceUtil::Shared
{
friend class EStopI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    EStopImpl( AbstractEStopCallback           &callback,
               const orca::EStopDescription    &descr,
               const std::string               &interfaceTag, 
               const orcaice::Context          &context );
    //! constructor using interfaceName
    EStopImpl( AbstractEStopCallback          &callback,
               const orca::EStopDescription   &descr,
               const orcaice::Context         &context,
               const std::string              &interfaceName );
    ~EStopImpl();
    
    // local functions
    //! may throw gbxutilacfr::Exceptions
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the activity (e.g. thread) was stopped.
    void initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::EStopData& data )
    {
        dataStore_.set( data );
    }

    orca::EStopDescription getDescription() const
        { return description_; }

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::EStopData &data );

private:
    void init();

    // remote call implementations, mimic (but do not inherit) the orca interface
    orca::EStopDescription internalGetDescription() const
        { return description_; }
    ::orca::EStopData internalGetData();
    IceStorm::TopicPrx internalSubscribe(const orca::EStopConsumerPrx& subscriber);

    gbxiceutilacfr::Store<orca::EStopData> dataStore_;

    typedef orcaice::TopicHandler<orca::EStopConsumerPrx,orca::EStopData> EStopTopicHandler;
    std::auto_ptr<EStopTopicHandler> topicHandler_;

    AbstractEStopCallback &callback_;
    const orca::EStopDescription description_;

    Ice::ObjectPtr ptr_;
    const std::string interfaceName_;
    orcaice::Context context_;
};
typedef IceUtil::Handle<EStopImpl> EStopImplPtr;

}

#endif
