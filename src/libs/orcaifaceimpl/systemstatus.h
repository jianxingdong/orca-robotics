/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_SYSTEMSTATUS_H
#define ORCAIFACEIMPL_SYSTEMSTATUS_H

#include <orca/systemstatus.h>
#include <orcaifaceutil/systemstatus.h>

#include <orcaifaceimpl/providerImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderImpl<orca::SystemStatus, orca::SystemStatusConsumer,
                    orca::SystemStatusData> SystemStatusImpl;
typedef IceUtil::Handle<SystemStatusImpl> SystemStatusImplPtr;

typedef StoringConsumerImpl<orca::SystemStatus, orca::SystemStatusConsumer,
                    orca::SystemStatusData> StoringSystemStatusConsumerImpl;
typedef IceUtil::Handle<StoringSystemStatusConsumerImpl> StoringSystemStatusConsumerImplPtr;

typedef BufferedConsumerImpl<orca::SystemStatus, orca::SystemStatusConsumer,
                        orca::SystemStatusData> BufferedSystemStatusConsumerImpl;
typedef IceUtil::Handle<BufferedSystemStatusConsumerImpl> BufferedSystemStatusConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::SystemStatus, orca::SystemStatusConsumer,
                        orca::SystemStatusData> NotifyingSystemStatusConsumerImpl;
typedef IceUtil::Handle<NotifyingSystemStatusConsumerImpl> NotifyingSystemStatusConsumerImplPtr;

typedef PrintingConsumerImpl<orca::SystemStatus, orca::SystemStatusConsumer,
                        orca::SystemStatusData> PrintingSystemStatusConsumerImpl;
typedef IceUtil::Handle<PrintingSystemStatusConsumerImpl> PrintingSystemStatusConsumerImplPtr;

}

#endif
