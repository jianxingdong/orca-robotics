/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_PROPERTIES_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_PROPERTIES_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orcaifaceimpl/properties.h>

namespace orcaprobefactory
{

class PropertiesProbe : public orcaprobe::InterfaceProbe
{

public:

    PropertiesProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context & context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData & data );

private:

    int loadGetData( orcacm::OperationData & data );
    int loadSetData( orcacm::OperationData & data );
    int loadSubscribe( orcacm::OperationData & data );
    int loadUnsubscribe( orcacm::OperationData & data );

    orcaifaceimpl::PrintingPropertiesConsumerImplPtr consumer_;
};

} // namespace

#endif
