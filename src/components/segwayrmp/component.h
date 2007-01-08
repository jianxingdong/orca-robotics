/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_COMPONENT_H
#define ORCA2_SEGWAY_RMP_COMPONENT_H

#include <orcaice/component.h>

// external interface definitions
#include <orca/odometry2d.h>
#include <orca/velocitycontrol2d.h>
#include <orca/odometry3d.h>
#include <orca/power.h>

// data pipes
#include <orcaice/proxy.h>
#include <orcaice/notify.h>

namespace segwayrmp
{

class HwHandler;
class NetHandler;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // component interface
    virtual void  start();
    virtual void stop();

private:
    // loop responsible for interaction with the network
    NetHandler     *netHandler_;
    // loop responsible for interaction with local hardware
    HwHandler      *hwHandler_;

    //
    // INTERFACES BETWEEN NETWORK AND HARDWARE HANDLERS
    //
    // hardware->network
    orcaice::Proxy<orca::Odometry2dData> odometry2dPipe_;
    orcaice::Proxy<orca::Odometry3dData> odometry3dPipe_;
    orcaice::Proxy<orca::PowerData> powerPipe_;

    // network->hardware
    orcaice::Notify<orca::VelocityControl2dData> commandPipe_;
};

} // namespace

#endif
