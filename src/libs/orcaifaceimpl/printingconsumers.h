/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_PRINTING_CONSUMERS_H
#define ORCAIFACEIMPL_PRINTING_CONSUMERS_H

#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl
{

//
// An attempt to minimise template kung-fu in component code.
//

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::RangeScanner2d, orca::RangeScanner2dPrx,
                        orca::RangeScanner2dConsumer, orca::RangeScanner2dConsumerPrx,
                        orca::RangeScanner2dDataPtr>       PrintingRangeScanner2dConsumerImpl;
typedef IceUtil::Handle<PrintingRangeScanner2dConsumerImpl> PrintingRangeScanner2dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::LaserScanner2d, orca::LaserScanner2dPrx,
                        orca::RangeScanner2dConsumer, orca::RangeScanner2dConsumerPrx,
                        orca::RangeScanner2dDataPtr>       PrintingLaserScanner2dConsumerImpl;
typedef IceUtil::Handle<PrintingLaserScanner2dConsumerImpl> PrintingLaserScanner2dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Localise2d, orca::Localise2dPrx,
                        orca::Localise2dConsumer, orca::Localise2dConsumerPrx,
                        orca::Localise2dData>              PrintingLocalise2dConsumerImpl;
typedef IceUtil::Handle<PrintingLocalise2dConsumerImpl>     PrintingLocalise2dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Localise3d, orca::Localise3dPrx,
                        orca::Localise3dConsumer, orca::Localise3dConsumerPrx,
                        orca::Localise3dData>              PrintingLocalise3dConsumerImpl;
typedef IceUtil::Handle<PrintingLocalise3dConsumerImpl>     PrintingLocalise3dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Odometry2d, orca::Odometry2dPrx,
                        orca::Odometry2dConsumer, orca::Odometry2dConsumerPrx,
                        orca::Odometry2dData>              PrintingOdometry2dConsumerImpl;
typedef IceUtil::Handle<PrintingOdometry2dConsumerImpl>     PrintingOdometry2dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Odometry3d, orca::Odometry3dPrx,
                        orca::Odometry3dConsumer, orca::Odometry3dConsumerPrx,
                        orca::Odometry3dData>              PrintingOdometry3dConsumerImpl;
typedef IceUtil::Handle<PrintingOdometry3dConsumerImpl>     PrintingOdometry3dConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::DriveBicycle, orca::DriveBicyclePrx,
                        orca::DriveBicycleConsumer, orca::DriveBicycleConsumerPrx,
                        orca::DriveBicycleData>            PrintingDriveBicycleConsumerImpl;
typedef IceUtil::Handle<PrintingDriveBicycleConsumerImpl>   PrintingDriveBicycleConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::OgMap, orca::OgMapPrx,
                        orca::OgMapConsumer, orca::OgMapConsumerPrx,
                        orca::OgMapData>                   PrintingOgMapConsumerImpl;
typedef IceUtil::Handle<PrintingOgMapConsumerImpl>          PrintingOgMapConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Gps, orca::GpsPrx,
                        orca::GpsConsumer, orca::GpsConsumerPrx,
                        orca::GpsData>                     PrintingGpsConsumerImpl;
typedef IceUtil::Handle<PrintingGpsConsumerImpl>            PrintingGpsConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Status, orca::StatusPrx,
                        orca::StatusConsumer, orca::StatusConsumerPrx,
                        orca::StatusData>                      PrintingStatusConsumerImpl;
typedef IceUtil::Handle<PrintingStatusConsumerImpl>            PrintingStatusConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::EStop, orca::EStopPrx,
                        orca::EStopConsumer, orca::EStopConsumerPrx,
                        orca::EStopData>                      PrintingEStopConsumerImpl;
typedef IceUtil::Handle<PrintingEStopConsumerImpl>            PrintingEStopConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Image, orca::ImagePrx,
                        orca::ImageConsumer, orca::ImageConsumerPrx,
                        orca::ImageDataPtr>                      PrintingImageConsumerImpl;
typedef IceUtil::Handle<PrintingImageConsumerImpl>            PrintingImageConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Camera, orca::CameraPrx,
                        orca::ImageConsumer, orca::ImageConsumerPrx,
                        orca::ImageDataPtr>                      PrintingCameraConsumerImpl;
typedef IceUtil::Handle<PrintingCameraConsumerImpl>            PrintingCameraConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Power, orca::PowerPrx,
                        orca::PowerConsumer, orca::PowerConsumerPrx,
                        orca::PowerData>                      PrintingPowerConsumerImpl;
typedef IceUtil::Handle<PrintingPowerConsumerImpl>            PrintingPowerConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Properties, orca::PropertiesPrx,
                        orca::PropertiesConsumer, orca::PropertiesConsumerPrx,
                        orca::PropertiesData>                      PrintingPropertiesConsumerImpl;
typedef IceUtil::Handle<PrintingPropertiesConsumerImpl>            PrintingPropertiesConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Tracer, orca::TracerPrx,
                        orca::TracerConsumer, orca::TracerConsumerPrx,
                        orca::TracerData>                      PrintingTracerConsumerImpl;
typedef IceUtil::Handle<PrintingTracerConsumerImpl>            PrintingTracerConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Wifi, orca::WifiPrx,
                        orca::WifiConsumer, orca::WifiConsumerPrx,
                        orca::WifiData>                      PrintingWifiConsumerImpl;
typedef IceUtil::Handle<PrintingWifiConsumerImpl>            PrintingWifiConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::Ins, orca::InsPrx,
                        orca::InsConsumer, orca::InsConsumerPrx,
                        orca::InsData>                     PrintingInsConsumerImpl;
typedef IceUtil::Handle<PrintingInsConsumerImpl>            PrintingInsConsumerImplPtr;


//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::PointCloud, orca::PointCloudPrx,
                        orca::PointCloudConsumer, orca::PointCloudConsumerPrx,
                        orca::PointCloudData>           PrintingPointCloudConsumerImpl;
typedef IceUtil::Handle<PrintingPointCloudConsumerImpl> PrintingPointCloudConsumerImplPtr;

//! Typedef for a common interface type. There's also a corresponding Ptr.
typedef PrintingConsumerImpl<orca::SystemStatus, orca::SystemStatusPrx,
                        orca::SystemStatusConsumer, orca::SystemStatusConsumerPrx,
                        orca::SystemStatusData>            PrintingSystemStatusConsumerImpl;
typedef IceUtil::Handle<PrintingSystemStatusConsumerImpl>  PrintingSystemStatusConsumerImplPtr;


} // namespace

#endif
