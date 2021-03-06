/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_DEFAULT_PROBE_FACTORY_H
#define ORCA_DEFAULT_PROBE_FACTORY_H

#include <orcaprobe/factory.h>

namespace orcaprobefactory
{

class OrcaProbeFactory : public orcaprobe::Factory
{

public:
    OrcaProbeFactory();

    virtual orcaprobe::InterfaceProbe* create( 
                const std::string           & interfaceType, 
                const orca::FQInterfaceName & name, 
                const Ice::ObjectPrx        & adminPrx,
                orcaprobe::AbstractDisplay  & display, 
                const orcaice::Context      & context );

};

} // namespace

extern "C" {
    orcaprobe::Factory *createFactory();
}

#endif
