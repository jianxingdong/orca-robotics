/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACER_INTERFACE_ICE
#define ORCA2_TRACER_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/ocm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_tracer Tracer
    @brief Remote access to component's traces, e.g. errors, warning, etc.
    @{

    The Tracer routes different types of information to different destinations.
*/

//! Types of traced information
enum TraceType {
    //! Information
    InfoTrace,
    //! Warning
    WarningTrace,
    //! Error
    ErrorTrace,
    //! Debug statement
    DebugTrace
};

/*!
    @brief Component status data.
*/
struct TracerData
{   
    //! Time when the trace was produced.
    Time timeStamp;

    //! The fully-qualified name of the interface.
    FQComponentName name;

    //! Trace type.
    TraceType type;

    //! Verbosity level, 1 being the lowest
    int verbosity;

    //! Status message from component.
    string message;
};


/*!
 *
 * Data consumer interface (needed only for the push pattern).
 *
 */
interface TracerConsumer
{
    //! This is function envoked by the publisher of traces on the consumer of traces.
    void setData( TracerData obj );
};

/*!
    @brief Verbosity levels for different trace types.

    When setting desired verbosity level, any value can be set to -1 to leave the current value unchanged.
*/
struct TracerVerbosityConfig
{   
    //! Verbosity level for error traces.
    int error;

    //! Verbosity level for warning traces.
    int warning;

    //! Verbosity level for information traces.
    int info;

    //! Verbosity level for debug traces.
    int debug;
};

/*!
 *  @brief Routes different types of information to different destinations.
 */
interface Tracer
{
    //! Get current verbosity level for traces transmitted over the network.
    idempotent TracerVerbosityConfig getVerbosity();

    //! Set verbosity level for traces transmitted over the network.
    idempotent void setVerbosity( TracerVerbosityConfig verbosity );

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * This function will subscribe the subscriber for all tracer messages
     * generated by this component.
     *
     * @see unsubscribe
     */
    void subscribeForComponentMessages( TracerConsumer* subscriber )
        throws SubscriptionFailedException;

    /*!
     *
     * This will subscribe for all info messages sent by any component
     * on this platform.
     *
     * Well, actually it may be slightly more complicated: if your
     * platform has more than one instance of IceStorm, this function
     * will subscribe to all info messages sent to the same instance
     * of IceStorm as this component.
     *
     */
    void subscribeForPlatformInfoMessages( TracerConsumer* subscriber )
        throws SubscriptionFailedException;

    /*!
     *
     * This will subscribe for all warning messages sent by any component
     * on this platform.
     *
     * Well, actually it may be slightly more complicated: if your
     * platform has more than one instance of IceStorm, this function
     * will subscribe to all warning messages sent to the same instance
     * of IceStorm as this component.
     *
     */
    void subscribeForPlatformWarningMessages( TracerConsumer* subscriber )
        throws SubscriptionFailedException;

    /*!
     *
     * This will subscribe for all error messages sent by any component
     * on this platform.
     *
     * Well, actually it may be slightly more complicated: if your
     * platform has more than one instance of IceStorm, this function
     * will subscribe to all error messages sent to the same instance
     * of IceStorm as this component.
     *
     */
    void subscribeForPlatformErrorMessages( TracerConsumer* subscriber )
        throws SubscriptionFailedException;
    
    /*!
     * Unsubscribe an existing subscriber from component messages
     *
     * @see subscribe
     */
    idempotent void unsubscribeForComponentMessages( TracerConsumer* subscriber );

    /*!
     * Unsubscribe an existing subscriber from platform info messages
     *
     * @see subscribe
     */
    idempotent void unsubscribeForPlatformInfoMessages( TracerConsumer* subscriber );

    /*!
     * Unsubscribe an existing subscriber from platform warning messages
     *
     * @see subscribe
     */
    idempotent void unsubscribeForPlatformWarningMessages( TracerConsumer* subscriber );

    /*!
     * Unsubscribe an existing subscriber from platform error messages
     *
     * @see subscribe
     */
    idempotent void unsubscribeForPlatformErrorMessages( TracerConsumer* subscriber );

};


/*!
    @}
*/
}; // module

#endif
