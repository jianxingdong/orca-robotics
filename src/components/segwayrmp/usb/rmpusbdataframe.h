/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
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

// MESSAGE ID'S

// Segway->CPU messages
// warning: Segway manual refers to them as Message 1-7
// we call them Message 0-6 to synch with 0x0400-406
// 0x0400 = 1024
#define RMP_CAN_ID_MSG0         0x0400
#define RMP_CAN_ID_MSG1         0x0401
#define RMP_CAN_ID_MSG2         0x0402
#define RMP_CAN_ID_MSG3         0x0403
#define RMP_CAN_ID_MSG4         0x0404
#define RMP_CAN_ID_MSG5         0x0405
#define RMP_CAN_ID_MSG6         0x0406
#define RMP_CAN_ID_MSG7         0x0407

// CPU->Segway message
#define RMP_CAN_ID_SHUTDOWN     0x0412

// CPU->Segway message
#define RMP_CAN_ID_COMMAND      0x0413

// CU->UI: CU Status message
#define RMP_CAN_ID_STATUS       0x0680

// UI->CU: RMP Heartbeat message
#define RMP_CAN_ID_HEARTBEAT    0x0688

// DATA MASKS

// Mask for user interface battery voltage in the heartbeat message.
#define RMP_CAN_MASK_HEARTBEAT_UIBAT_VOLTAGE    0x03F0

// Mask for user interface battery status in the heartbeat message.
#define RMP_CAN_MASK_HEARTBEAT_UIBAT_STATUS     0xC000

// from Configuration Command table, see sec.2.2.1
#define RMP_CMD_NONE                        0
#define RMP_CMD_SET_MAXIMUM_VELOCITY        10
#define RMP_CMD_SET_MAXIMUM_ACCELERATION    11
#define RMP_CMD_SET_MAXIMUM_TURN_RATE       12
#define RMP_CMD_SET_GAINMODE                13
#define RMP_CMD_SET_CURRENT_LIMIT           14
#define RMP_CMD_SET_BALANCE_LOCKOUT         15
#define RMP_CMD_SET_OPERATIONAL_MODE        16
#define RMP_CMD_RESET_INTEGRATORS           50

// from table: Bitfield for Reset Integrators, see sec.2.2.2
#define RMP_CAN_RESET_RIGHT           0x01
#define RMP_CAN_RESET_LEFT            0x02
#define RMP_CAN_RESET_YAW             0x04
#define RMP_CAN_RESET_FOREAFT         0x08
#define RMP_CAN_RESET_ALL             (RMP_CAN_RESET_RIGHT | \
                                      RMP_CAN_RESET_LEFT | \
                                      RMP_CAN_RESET_YAW | \
                                      RMP_CAN_RESET_FOREAFT)

// unit conversions from Data Reference, see sec.4

#define RMP_COUNT_PER_M            33215.0
#define RMP_COUNT_PER_M_PER_S      332.0

#define RMP_COUNT_PER_DEG          7.8
#define RMP_COUNT_PER_DEG_PER_S    7.8
#define RMP_COUNT_PER_DEG_PER_SS   7.8
#define RMP_COUNT_PER_RAD          446.9071
#define RMP_COUNT_PER_RAD_PER_S    446.9071
#define RMP_COUNT_PER_RAD_PER_SS   446.9071
// This is different from rotation and depends on geometry
#define RMP_COUNT_PER_REV          112644.0

// main (CU) battery voltage
#define RMP_BASE_COUNT_PER_VOLT      4.0

// user interface battery voltage: volts=offset + n*counts
#define RMP_UI_OFFSET               1.4
#define RMP_UI_COEFF                0.0125
// user interface battery status: low battery warning
#define RMP_UI_LOW_WARNING          0x80
// user interface battery status: empty battery shutdown
#define RMP_UI_EMPTY_SHUTDOWN       0x40

// motor torque
#define RMP_COUNT_PER_NM           1094.0
#define RMP_SEC_PER_FRAME          0.01

// alexm: where is this from?
#define RMP_MAX_TRANS_VEL_MM_S      3576
// from rmi_demo: 1300*0.013805056
#define RMP_MAX_ROT_VEL_DEG_S       18

// limits

// from page 7 of the manual  
#define RMP_MAX_TRANS_VEL_COUNT     1176
#define RMP_MAX_ROT_VEL_COUNT       1024

// minimum powerbase (CU) battery voltage (V)
#define RMP_BASE_MIN_VOLTAGE          66.0
// minimum user interface (UI) battery voltage (V)
#define RMP_UI_MIN_VOLTAGE          6.0

// maximum time without incoming commands [s]
#define RMP_COMMAND_TIMEOUT         0.4

// misc
#define RMP_GEOM_WHEEL_SEP          0.54

#define RMP_CAN_DROPPED_PACKET      0x5A5A

// need it for the CanPacket definition
#include "canpacket.h"

// this holds all the RMP data it gives us
class RmpUsbDataFrame
{
public:
    RmpUsbDataFrame();

    // Adds information from a packet
    void AddPacket(const CanPacket* pkt);

    // Print out frame data to screen
    void dump();

    // Print out frame data to string
    std::string toString();

    // Allows adding more packets to the frame without resetting the data
    void reopen();

    // Resets all data to RMP_CAN_DROPPED_PACKET
    void reset();

    // Did the next frame start?
    bool isClosed() const;
    // Did all the packets arrive?
    bool isComplete() const;

    // Build ID
    int16_t    build_id;
    // Pitch Angle
    int16_t    pitch;
    // Pitch Rate
    int16_t    pitch_dot;
    // Roll Angle
    int16_t    roll;
    // Roll Rate
    int16_t    roll_dot;
    // yaw displacement
    uint32_t   yaw;
    // Yaw rate
    int16_t    yaw_dot;
    // wheel displacement left
    uint32_t   left;
    // Left wheel Velocity
    int16_t    left_dot;
    // wheel displacement right
    uint32_t   right;
    // Right wheel Velocity
    int16_t    right_dot;
    // fore/aft displacement
    uint32_t   foreaft;
    // Left Motor Torque
    int16_t    left_torque;
    // Right Motor Torque
    int16_t    right_torque;
    // Servo Frames
    uint16_t   frames;
    // User Interface battery voltage
    uint16_t   ui_battery_voltage;
    // User Interface battery status
    //uint16_t   ui_battery_status;
    // Powerbase attery Voltage (min of A, B)
    uint16_t   base_battery_voltage;
    // Operational Mode (0= disabled, 1=tractor, 2= balance)
    uint16_t operational_mode;
    // Controller Gain Schedule (0=normal, 1=tall, 2=heavy)
    uint16_t controller_gain_schedule;
    // Velocity command (as received)
    int16_t velocity_command;
    // Turn Command (as received)
    int16_t rate_command;

    // CU Status "word 1"
    int16_t status_word1;
    // CU Status "word 2"
    int16_t status_word2;

    bool msgCheckList_[8];

    //debug
    bool kill_flag;
    // ui battery info from the heartbeat msg
    uint16_t   ui_heartbeat_voltage;
    uint16_t   ui_heartbeat_status;

};


