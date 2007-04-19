/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAIFACEIMPL_LOCALISE3D_IFACE_H
#define ORCA2_ORCAIFACEIMPL_LOCALISE3D_IFACE_H

#include <orca/localise3d.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the Localise3d interface: Handles remote calls.
//!
class Localise3dIface : public IceUtil::Shared
{
public:
    Localise3dIface( const std::string &ifaceTag,
                     const orcaice::Context &context );
    ~Localise3dIface();

    // remote calls:

    ::orca::Localise3dData getData() const;

    void subscribe(const ::orca::Localise3dConsumerPrx&);

    void unsubscribe(const ::orca::Localise3dConsumerPrx&);

    // Note: could do smoothing/interpolation here...
    orca::Localise3dData getDataAtTime(const orca::Time&) const
        { return getData(); }

    // Local calls:

    //! may throw orcaice::Exceptions
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::Localise3dData &data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::Localise3dData &data );

private:

    orcaice::Proxy<orca::Localise3dData> dataProxy_;

    orca::Localise3dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string              ifaceTag_;
    orcaice::Context               context_;
};
typedef IceUtil::Handle<Localise3dIface> Localise3dIfacePtr;

} // namespace

#endif