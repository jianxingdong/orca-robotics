/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_POWER_INTERFACE_ICE
#define ORCA2_POWER_INTERFACE_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_power Power
    @brief Access to a robot's power subsystem
    @{
*/

/*!
    Data for a single battery.
*/
struct BatteryData
{
    //! Battery name
    string name;
    //! Battery voltage [V]
    float voltage;
    //! Percent of full charge [%]
    float percent;
};

/*!
    Data for all batteries on a robot.
*/
sequence<BatteryData> BatteriesData;

/*!
    @brief Information about the power subsystem of a robot.
*/
class PowerData extends OrcaObject
{
    //! battery information
    BatteriesData batteries;
};

/*!
    @brief Access to a robot's power subsystem
*/
interface Power
{
        //! ClientPull_Supplier interface
        nonmutating PowerData getData();
};

/*!
    @brief Interface to the consumer of power information.
*/
interface PowerConsumer
{
        //! ServerPush_Consumer interface
       void setData( PowerData obj );
};


/*!
    @}
*/
}; // module

#endif
