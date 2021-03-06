/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_WIFI_ICE
#define ORCA2_WIFI_ICE

#include <orca/common.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_wifi Wifi
    @brief Wireless interfaces information

    @{
*/
    
//! Interpretation of link quality
enum LinkQualityType
{
    //! link quality type is unknown
    LinkQualityTypeUnknown,
    //! link quality is in dBm
    LinkQualityTypeDbm,
    //! link quality is relative
    LinkQualityTypeRelative
};

//! A 'Windows-style' signal level
enum DiscreteSignalLevel
{
    //! signal level is unknown
    SignalLevelUnknown,
    //! signal level is very low
    SignalLevelVeryLow,
    //! signal level is low
    SignalLevelLow,
    //! signal level is good
    SignalLevelGood,
    //! signal level is very good
    SignalLevelVeryGood,
    //! signal level is excellent
    SignalLevelExcellent
};

//! Operation modes
enum OperationMode
{
    //! unknown operating mode
    OperationModeUnknown,
    //! driver decides the mode
    OperationModeAuto,
    //! ad hoc mode
    OperationModeAdhoc,
    //! infrastructure mode (multi cell network, roaming)
    OperationModeInfrastructure,
    //! access point, master mode
    OperationModeMaster,
    //! repeater mode
    OperationModeRepeat,
    //! secondary/backup repeater
    OperationModeSecondRepeater
};

/*!
 *  @brief Contains information per wireless interface, e.g. eth1.
 * 
 *  All values are driver specific. This data structure resembles information from /proc/net/wireless
 *  plus some information obtained from the kernel.
 *  Read man-page of iwconfig for detailed documentation of all fields.
 * 
 */
struct WifiInterface
{
    //! Interface name, e.g. eth1
    string interfaceName;
    //! Status (device-dependent)
    int status;
    //! Link quality
    int linkQuality;
    //! Signal level
    int signalLevel;
    //! Noise level
    int noiseLevel;
    //! Rx invalid nwid: number of packets received with a different NWID or ESSID.
    int numInvalidNwid;
    //! Rx invalid crypt: number of packets hardware was unable to decrypt
    int numInvalidCrypt;
    //! Rx invalid frag: number of packets not properly reassembled
    int numInvalidFrag;
    //! Tx excessive retries: number of packets hardware failed to deliver
    int numRetries;
    //! Invalid misc: other lost packets
    int numInvalidMisc;
    //! Missed beacon: number of periodic beacons from cell/access point we have missed
    int numMissedBeacons;
    
    //! Operating mode (see above)
    OperationMode mode;
    //! Bitrate in bits/second
    int bitrate;
    //! Access point MAC address
    string accessPoint;
    
    //! Throughput (TODO: what does it mean? unit?)
    int throughPut;
    //! How to interpret the link quality (see above)
    LinkQualityType linkType;
    //! Discrete signal level indicator
    DiscreteSignalLevel discreteLevel;
    //! Maximum link quality
    int maxLinkQuality;
    //! Maximum signal level
    int maxSignalLevel;
    //! Maximum noise level
    int maxNoiseLevel;
    
};

//! A data structure which keeps individual wifi interfaces
sequence<WifiInterface> WifiInterfaces;

/*!
    @brief Wireless signal data
*/
struct WifiData
{
    //! Time when data was measured.
    Time timeStamp;
    //! A sequence of wireless interfaces
    WifiInterfaces interfaces;
};

/*!
    @brief Subscriber to Wifi information.
*/
interface WifiConsumer
{
    //! Set data
    void setData( WifiData obj );
};

//! Interface to Wifi information.
interface Wifi
{
    //! Get current Wifi data. Raises DataNotExistException if data is not available.
    idempotent WifiData getData()
        throws DataNotExistException;

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( WifiConsumer* subscriber )
        throws SubscriptionFailedException;
};

/*! @} */
}; // module

#endif
