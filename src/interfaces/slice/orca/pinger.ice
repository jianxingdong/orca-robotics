#ifndef ORCA2_ROVERHARDWARE_INTERFACE_ICE
#define ORCA2_ROVERHARDWARE_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/power.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_pinger Pinger
    @brief An interface which provides the well-known 'ping' service.
    @{
*/

//! Thrown when something goes wrong
exception PingException extends OrcaException {};

/*!
    @brief An interface which provides the well-known 'ping' service.
           
    This is provided as an ice interface because 'ping' requires root
    priveliges, so it's hard to do from within a large C++ program
    (assuming you don't want to run the entire thing as root).  The
    'pinger' interface allows a small component to be run with root
    priveliges, and provide this service to other non-superuser
    components.

    AlexB: Seems like a bit of a heavy-handed approach, but I'm not
    sure how else to do it...  An obvious possibility is
    system("ping") and use some sed/grep magic to pull out the numbers
    and write to a file, then read the file.  This might work but (a)
    it's brittle and a pain in the arse, and (b) it's hard to get
    meaningful errors if something goes wrong.
*/
interface Pinger
{
    //!
    //! Returns:
    //!   >=0 : a latency in milliseconds
    //!   <0  : ping timed out.
    //!
    //! WARNING: This thing executes in the caller's thread.
    //!          So it may wait for the ping to time out before returning.
    //!          So, you should make sure that the Timeout value of the Ice call
    //!          is larger than the timeout value of the server's ping call.
    //!
    double ping( string hostname )
        throws PingException;
};


//!  //@}
}; // module

#endif
