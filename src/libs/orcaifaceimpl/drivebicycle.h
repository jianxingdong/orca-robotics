/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_DRIVEBICYCLE_H
#define ORCAIFACEIMPL_DRIVEBICYCLE_H

#include <orca/drivebicycle.h>
#include <orcaifaceutil/drivebicycle.h>

#include <orcaifaceimpl/drivebicycleImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef StoringConsumerImpl<orca::DriveBicycle, orca::DriveBicycleConsumer,
                    orca::DriveBicycleData> StoringDriveBicycleConsumerImpl;
typedef IceUtil::Handle<StoringDriveBicycleConsumerImpl> StoringDriveBicycleConsumerImplPtr;

typedef BufferedConsumerImpl<orca::DriveBicycle, orca::DriveBicycleConsumer,
                        orca::DriveBicycleData> BufferedDriveBicycleConsumerImpl;
typedef IceUtil::Handle<BufferedDriveBicycleConsumerImpl> BufferedDriveBicycleConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::DriveBicycle, orca::DriveBicycleConsumer,
                        orca::DriveBicycleData> NotifyingDriveBicycleConsumerImpl;
typedef IceUtil::Handle<NotifyingDriveBicycleConsumerImpl> NotifyingDriveBicycleConsumerImplPtr;

typedef PrintingConsumerImpl<orca::DriveBicycle, orca::DriveBicycleConsumer,
                        orca::DriveBicycleData> PrintingDriveBicycleConsumerImpl;
typedef IceUtil::Handle<PrintingDriveBicycleConsumerImpl> PrintingDriveBicycleConsumerImplPtr;

}

#endif
