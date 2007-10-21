/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAWALLFACTORY_VELOCITYCONTROL2D_INTERFACE_SIM_H
#define ORCA2_ORCAWALLFACTORY_VELOCITYCONTROL2D_INTERFACE_SIM_H

#include <orcawall/interfacesim.h>

namespace orcawallfactory
{

class VelocityControl2dSim : public orcawall::InterfaceSim
{

public:
    VelocityControl2dSim( const std::string& tag, const orcaice::Context& context );

    // from hydroutil::Thread
    virtual void run();
};

} // namespace

#endif
