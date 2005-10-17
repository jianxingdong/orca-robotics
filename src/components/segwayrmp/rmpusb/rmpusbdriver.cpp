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

#include <iostream>
#include <sys/time.h>

#include <rmpusbdriver.h>
#include "canio_usb_ftdi.h"
#include "rmpusb_frame.h"

#include <orcaiceutil/objutils.h>
#include <orcaiceutil/proputils.h>
#include <orcaiceutil/mathdefs.h>

using namespace std;
using namespace orca;
using namespace orcaiceutil;

SegwayRmpUsb::SegwayRmpUsb(
        orcaiceutil::PtrBuffer* position2dBuf, orcaiceutil::PtrBuffer* powerBuf,
        orcaiceutil::PtrProxy* position2dProxy, orcaiceutil::PtrProxy* commandProxy,
        orcaiceutil::PtrProxy* powerProxy ) :
                    position2dBuf_(position2dBuf), powerBuf_(powerBuf),
                    position2dProxy_(position2dProxy), commandProxy_(commandProxy),
                    powerProxy_(powerProxy)
{
    // init internal data storage
    position2dData_ = new Position2dData;
    commandData_ = new Velocity2dCommand;
    // set up data structure for 3 batteries
    powerData_ = new PowerData;
    BatteryData bd;
    for ( int i=0; i<3; ++i ) {
        powerData_->batteries.push_back( bd );
    }
    powerData_->batteries[0].name = "main-front";
    powerData_->batteries[1].name = "main-rear";
    powerData_->batteries[2].name = "io";
}

SegwayRmpUsb::~SegwayRmpUsb(){}

void SegwayRmpUsb::setup( const Ice::PropertiesPtr & properties )
{
    // Hardware
    canio_ = new CanioUsbFtdi;
    data_frame_ = new rmpusb_frame_t;

    //
    // Read settings
    //
    maxSpeed_ = orcaiceutil::getPropertyAsDoubleWithDefault( properties, "Rmp.MaxSpeed", 1.0 );
    maxTurnrate_ = orcaiceutil::getPropertyAsDoubleWithDefault( properties, "Rmp.MaxTurnrate", 40.0 )*DEG2RAD_RATIO;
    cout<<"properties: maxspeed="<<maxSpeed_<<", maxturn="<<maxTurnrate_<<endl;
}

void SegwayRmpUsb::activate()
{
    cout<<"activating device"<<endl;

    // init device (freq is not used)
    if ( canio_->Init(1)<0 )
    {
        cerr<<"ERROR: error on USB Init"<<endl;
        exit(1);
    }

    // Initialize odometry
    odomX_ = 0.0;
    odomY_ = 0.0;
    odomYaw_ = 0.0;
    lastRawYaw_ = 0;
    lastRawLeft_ = 0;
    lastRawRight_ = 0;
    lastRawForeaft_ = 0;

    //lastSpeedX_ = 0;
    //lastSpeedYaw_ = 0;
    motor_allow_enable = false;
    motor_enabled = false;
    firstread_ = true;

    // start the driver thread, will call run()
    this->start();
}

void SegwayRmpUsb::deactivate()
{
    cout<<"deactivating device"<<endl;

    // tell the driver thread to stop
    stop();
}

void SegwayRmpUsb::run()
{
    cout<<"SegwayRmpUsb::main: starting nicely"<<endl;

    // Variables used in the loop
    CanPacket pkt;
    //int32_t xspeed,yawspeed;
    bool first = true;

    // Variables for the write timer
    int       timeSinceLastWrite;
    timeval   writeTime, priorWriteTime;
    const double maxUsbWriteFreq = 20.0;
    const int minWritePeriodUsec = (int)(1e6/maxUsbWriteFreq);

    // HACK: magic number for reset()
    canio_->Init( 888 );

    // Initialize write timer
    gettimeofday( &writeTime, NULL );
    gettimeofday( &priorWriteTime, NULL );

    while( isActive() )
    {
        // Read from the RMP into internal storage
        if ( read() < 0 ) {
            cerr<<"read() errored; bailing"<<endl;
            //! @todo shouldn't just quit here
            stop();
        }
        
        // Note that we got some data
        if (first) {
            first = false;
            cout<<"debug: got data from rmp"<<endl;
        }
                
        // TODO: report better timestamps, possibly using time info from the RMP

        // make a copy of internal storage into driver/component interface
        // this will be returned to pulling clients
        position2dProxy_->set( position2dData_ );
        powerProxy_->set( powerData_ );
        // this will be pushed to IceStorm
        position2dBuf_->push( position2dData_ );
        powerBuf_->push( powerData_ );

        // check for incoming commands
        if ( commandProxy_->isNewData() )
        {
            // Calculate how much time has elapsed since the last write. If it's less
            // than the minimum we specified, then skip the write step. The maximum
            // frequency we are willing to write to the USB is defined as MAX_USB_WRITE_FREQ.
            gettimeofday( &writeTime, NULL );
            timeSinceLastWrite = (int) ((writeTime.tv_sec - priorWriteTime.tv_sec)*1e6 +
                    (writeTime.tv_usec - priorWriteTime.tv_usec));
            if ( timeSinceLastWrite>300000 ) {
                cout<<"got late command, "<<timeSinceLastWrite<<"/"<<minWritePeriodUsec<<endl;
            }

            if ( timeSinceLastWrite >= minWritePeriodUsec )
            {                
                Ice::ObjectPtr data;
                commandProxy_->get( data );
                commandData_ = Velocity2dCommandPtr::dynamicCast( data );
                //cout<<"comm : "<<commandData_<<endl;        

                // Make velocity command (keep it in mm for now, so not to modify makeVelocityCommand() )
                //xspeed = (int)floor( commandData_->speed*1000.0 );
                //yawspeed = (int)floor( commandData_->turnrate / DEG2RAD_RATIO );
                makeVelocityCommand( &pkt );
            
                //if ( xspeed || yawspeed ) printf("writing non-zero velocity command...\n");
                if ( write(pkt) < 0 )
                {
                    cerr<<"SegwayRmpUsb::main: error on write"<<endl;
                    exit(1);
                }
            
                //printf("Write freq = %i Hz\n", (int)(1e6/timeSinceLastWrite) );
                
                // Reset timer
                gettimeofday( &priorWriteTime, NULL );
            }

        }

        // Throttle the read/write cycles to conserve cpu time. Note that this
        // only determines how fast we execute the main loop. The frequency with which we
        // write to the USB is still capped by MAX_USB_WRITE_FREQ and the timing code
        // surrounding the write function in main().
        // Ref: 1 kHz => approx 1000 usec sleep
        //      100 Hz => approx 10000 usec sleep
        usleep(1000);
    }

    // we are deactiveated

}

int SegwayRmpUsb::read()
{
    CanPacket pkt;
    int packetsRead;

    //! @todo get rid of this mess
    // chip: This is a temporary way to get at our new functions
    CanioUsbFtdi* pCanio = (CanioUsbFtdi*)canio_;

    // Read packets
    //cout<<"reading packets..."<<endl;
    packetsRead = pCanio->ReadPackets();
    
    // Process data packets    
    while( pCanio->GetNextPacket(pkt) )
    {
        // Add packet to data frame
        data_frame_->AddPacket(pkt);

        // chips's debugging stuff
        //WatchPacket( &pkt, 0x407 );
        //WatchDataStream( pkt );

        // If frame is complete, transfer data and reset frame
        if( data_frame_->is_ready )
        {
            updateData( data_frame_ );
            data_frame_->reset();
        }
    }

    //printf("End of read cycle.\n");

    return 0;
}

void SegwayRmpUsb::updateData( rmpusb_frame_t* data_frame )
{
    int delta_lin_raw, delta_ang_raw;
    double deltaX;
    double deltaYaw;
    //double tmp;

    // Use integrated values to calculate odometry
    // make sure the required fields were not dropped in this frame
    if (  data_frame->foreaft != RMP_CAN_DROPPED_PACKET  &&
          data_frame->yaw     != RMP_CAN_DROPPED_PACKET  )
    {
        // Get the new linear and angular encoder values and compute
        // odometry.  Note that we do the same thing here, regardless of
        // whether we're presenting 2D or 3D position info.
        delta_lin_raw = diff(lastRawForeaft_, data_frame->foreaft, firstread_);
        lastRawForeaft_ = data_frame->foreaft;
        
        delta_ang_raw = diff(lastRawYaw_, data_frame->yaw, firstread_);
        lastRawYaw_ = data_frame->yaw;
        
        deltaX = (double)delta_lin_raw / (double)RMP_COUNT_PER_M;
        deltaYaw = DEG2RAD((double)delta_ang_raw / (double)RMP_COUNT_PER_REV * 360.0);
        
        // First-order odometry integration
        odomX_ += deltaX * cos(odomYaw_);
        odomY_ += deltaX * sin(odomYaw_);
        odomYaw_ += deltaYaw;
        
        // Normalize yaw in [0, 360]
        odomYaw_ = atan2(sin(odomYaw_), cos(odomYaw_));
        if (odomYaw_ < 0) {
            odomYaw_ += 2 * M_PI;
        }

        // first, do 2D info.
        position2dData_->pose.p.x = odomX_;
        position2dData_->pose.p.y = odomY_;
        position2dData_->pose.o = odomYaw_;
    } else {
        //printf("skipping odometry\n");
    }

    //assert( this->position_data.xpos < 100 ); // when we miss msg5. why?

    // Use instanteneous values to calculate current rates
    // make sure the required fields were not dropped in this frame
    if ( data_frame->left_dot!=RMP_CAN_DROPPED_PACKET
         && data_frame->right_dot!=RMP_CAN_DROPPED_PACKET
         && data_frame->yaw_dot!=RMP_CAN_DROPPED_PACKET
         && data_frame->roll!=RMP_CAN_DROPPED_PACKET
         && data_frame->pitch!=RMP_CAN_DROPPED_PACKET
         && data_frame->roll_dot!=RMP_CAN_DROPPED_PACKET
         && data_frame->pitch_dot!=RMP_CAN_DROPPED_PACKET )
    {
        // combine left and right wheel velocity to get foreward velocity
        // change from counts/sec into meters/sec
        position2dData_->motion.v.x =
                ((double)data_frame->left_dot+(double)data_frame->right_dot) /
                    (double)RMP_COUNT_PER_M_PER_S / 2.0;
        
        // no side speeds for this bot
        position2dData_->motion.v.y = 0;
        
        // from counts/sec into deg/sec.  also, take the additive
        // inverse, since the RMP reports clockwise angular velocity as positive.
        position2dData_->motion.w = -(double)data_frame->yaw_dot / (double)RMP_COUNT_PER_DEG_PER_S;
        
        position2dData_->stalled = false;
/*        
        // now, do 3D info.
        this->position3d_data.xpos = htonl((int32_t)(odomX_ * 1000.0));
        this->position3d_data.ypos = htonl((int32_t)(odomY_ * 1000.0));
        // this robot doesn't fly
        this->position3d_data.zpos = 0;
        
        // normalize angles to [0,360]
        tmp = NORMALIZE(DTOR((double)data_frame->roll /
                (double)RMP_COUNT_PER_DEG));
        if(tmp < 0) {
            tmp += 2*M_PI;
        }
        this->position3d_data.roll = htonl((int32_t)rint(tmp * 1000.0));
        
        // normalize angles to [0,360]
        tmp = NORMALIZE(DTOR((double)data_frame->pitch /
                (double)RMP_COUNT_PER_DEG));
        if(tmp < 0) {
            tmp += 2*M_PI;
        }
        this->position3d_data.pitch = htonl((int32_t)rint(tmp * 1000.0));
        
        this->position3d_data.yaw = htonl(((int32_t)(odomYaw_ * 1000.0)));
        
        // combine left and right wheel velocity to get foreward velocity
        // change from counts/s into mm/s
        this->position3d_data.xspeed =
                htonl((uint32_t)rint(((double)data_frame->left_dot +
                (double)data_frame->right_dot) /
                (double)RMP_COUNT_PER_M_PER_S
                * 1000.0 / 2.0));
        // no side or vertical speeds for this bot
        this->position3d_data.yspeed = 0;
        this->position3d_data.zspeed = 0;
        
        this->position3d_data.rollspeed =
                htonl((int32_t)rint((double)data_frame->roll_dot /
                (double)RMP_COUNT_PER_DEG_PER_S * M_PI / 180 * 1000.0));
        this->position3d_data.pitchspeed =
                htonl((int32_t)rint((double)data_frame->pitch_dot /
                (double)RMP_COUNT_PER_DEG_PER_S * M_PI / 180 * 1000.00));
        // from counts/sec into millirad/sec.  also, take the additive
        // inverse, since the RMP reports clockwise angular velocity as
        // positive.
        
        // This one uses left_dot and right_dot, which comes from odometry
        this->position3d_data.yawspeed =
                htonl((int32_t)(rint((double)(data_frame->right_dot - data_frame->left_dot) /
                (RMP_COUNT_PER_M_PER_S * RMP_GEOM_WHEEL_SEP * M_PI) * 1000)));
        // This one uses yaw_dot, which comes from the IMU
        //data.position3d_data.yawspeed =
        //  htonl((int32_t)(-rint((double)data_frame->yaw_dot /
        //                        (double)RMP_COUNT_PER_DEG_PER_S * M_PI / 180 * 1000)));
        
        this->position3d_data.stall = 0;
*/
    } else {
        //printf("skipping rates\n");
    }

    // make sure the required fields were not dropped in this frame
    if ( data_frame->battery!=RMP_CAN_DROPPED_PACKET )
    {
        // Convert battery voltage to decivolts for Player.
        powerData_->batteries[0].voltage = data_frame->battery / RMP_COUNT_PER_VOLT;
        powerData_->batteries[0].percent = 99.0;
    } else {
        //printf("skipping power\n");
    }

    // Chip
    //data_frame->dump();
    //dump();

    firstread_ = false;
}

int SegwayRmpUsb::write(CanPacket& pkt)
{
    int ret;

    //printf("SENT: pkt: %s\n", pkt.toString());
    ret = canio_->WritePacket(pkt);
    
    return ret;
}

/* takes a player command (in host byte order) and turns it into CAN packets 
 * for the RMP 
 */
void SegwayRmpUsb::makeVelocityCommand( CanPacket* pkt )
{
    pkt->id = RMP_CAN_ID_COMMAND;
    pkt->PutSlot(2, (uint16_t)RMP_CAN_CMD_NONE);
  
    // we only care about cmd.xspeed and cmd.yawspeed
    // translational velocity is given to RMP in counts 
    // [-1176,1176] ([-8mph,8mph])
    
    // player is mm/s
    // 8mph is 3576.32 mm/s
    // so then mm/s -> counts = (1176/3576.32) = 0.32882963

    if ( commandData_->motion.v.x > maxSpeed_ )
    {
        cout<<"WARN: xspeed thresholded! ("<<commandData_->motion.v.x<<">"<<maxSpeed_<<")"<<endl;
        commandData_->motion.v.x = maxSpeed_;
    }
    else if(commandData_->motion.v.x < -maxSpeed_)
    {
        cout<<"WARN: xspeed thresholded! ("<<commandData_->motion.v.x<<"<"<<-maxSpeed_<<")"<<endl;
        commandData_->motion.v.x = -maxSpeed_;
    }

    //lastSpeedX_ = xspeed;

    // translational RMP command (convert m/s to mm/s first)
    int16_t trans = (int16_t) rint(commandData_->motion.v.x * 1e3 * (double)RMP_COUNT_PER_MM_PER_S);
    // check for command limits
    if(trans > RMP_MAX_TRANS_VEL_COUNT) {
        trans = RMP_MAX_TRANS_VEL_COUNT;
    }
    else if(trans < -RMP_MAX_TRANS_VEL_COUNT) {
        trans = -RMP_MAX_TRANS_VEL_COUNT;
    }

    if( commandData_->motion.w > maxTurnrate_ )
    {
        cout<<"WARN: yawspeed thresholded! ("<<commandData_->motion.w<<">"<<maxTurnrate_<<")"<<endl;
        commandData_->motion.w = maxTurnrate_;
    }
    else if( commandData_->motion.w < -maxTurnrate_ )
    {
        cout<<"WARN: yawspeed thresholded! ("<<commandData_->motion.w<<"<"<<-maxTurnrate_<<")"<<endl;
        commandData_->motion.w = -maxTurnrate_;
    }
  
    //lastSpeedYaw_ = yawspeed;

    // rotational RMP command \in [-1024, 1024] (convert from rad/s to deg/s first)
    // this is ripped from rmi_demo... to go from deg/s to counts
    // deg/s -> count = 1/0.013805056
    int16_t rot = (int16_t) rint(commandData_->motion.w / DEG2RAD_RATIO * (double)RMP_COUNT_PER_DEG_PER_S);
    // check for command limits
    if(rot > RMP_MAX_ROT_VEL_COUNT) {
        rot = RMP_MAX_ROT_VEL_COUNT;
    }
    else if(rot < -RMP_MAX_ROT_VEL_COUNT) {
        rot = -RMP_MAX_ROT_VEL_COUNT;
    }

    // put commands into the packet
    pkt->PutSlot(0, (uint16_t)trans);
    pkt->PutSlot(1, (uint16_t)rot);
}

// Calculate the difference between two raw counter values, taking care
// of rollover.
int SegwayRmpUsb::diff(uint32_t from, uint32_t to, bool first)
{
    int diff1, diff2;
    static uint32_t max = (uint32_t)pow(2.0,32.0)-1;

  // if this is the first time, report no change
    if(first)
        return(0);

    diff1 = to - from;

        // find difference in two directions and pick shortest
    if(to > from)
        diff2 = -(from + max - to);
    else
        diff2 = max - from + to;

    if(abs(diff1) < abs(diff2))
        return(diff1);
    else
        return(diff2);
}

void SegwayRmpUsb::WatchPacket( CanPacket* pkt, short int pktID )
{
    short slot0 = (short)pkt->GetSlot(0);
    short slot1 = (short)pkt->GetSlot(1);
    short slot2 = (short)pkt->GetSlot(2);
    short slot3 = (short)pkt->GetSlot(3);

    int slot0_lo = (int)pkt->GetSlot(0);
    int slot1_hi = (int)pkt->GetSlot(1) << 16;
    int slot2_lo = (int)pkt->GetSlot(2);
    int slot3_hi = (int)pkt->GetSlot(3) << 16;
                
    if( pkt->id == pktID )
    {
        //printf("SEGWAYIO: pkt: %s\n", pkt.toString());

        switch( pkt->id )
        {
            case 0x0401:
                printf("pitch = %6.2f, pitch rate = %6.2f,roll = %6.2f, roll rate = %6.2f\r",
                       (float)(slot0/7.8), (float)(slot1/7.8),
                       (float)(slot2/7.8), (float)(slot3/7.8));
                break;
            case 0x402:
                printf("LW vel = %6.2f, RW vel = %6.2f, yaw rate = %6.2f, frames = %8i\r",
                       (float)(slot0/332.0), float(slot1/332.0),
                       (float)(slot2/7.8), (int)(slot3/0.01)  );
                break;
            case 0x0403:
                printf("Left wheel = %6.2f, right wheel = %6.2f\r",
                       (float)((slot0_lo | slot1_hi)/33215.0),
                       (float)((slot2_lo | slot3_hi)/33215.0)   );
                break;
            case 0x0404:
                printf("Int f/a disp  = %6.2f, int yaw disp = %6.2f\r",
                       (float)((slot0_lo | slot1_hi)/33215.0),
                       (float)((slot2_lo | slot3_hi)/112644.0)   );
                break;
            case 0x0405:
                printf("Left motor torque  = %6.2f, right motor torque = %6.2f\r",
                       (float)(slot0/1094.0), (float)(slot1/1094.0)   );
                break;
            case 0x0406:
                printf("Op mode = %1i, gain sch = %1i, UI batt = %6.2f, Base batt = %6.2f\r",
                       slot0, slot1,
                       (float)(1.4 + slot2*0.0125), (float)(slot3/4.0) );
                break;
            case 0x0407:
                printf("Vel command = %4i, turn command = %4i\n",
                       slot0, slot1 );
                break;
        }
    }
}

void SegwayRmpUsb::WatchDataStream( CanPacket& pkt )
{
    static CanPacket priorPkt;

    // Check for breaks in the message sequence
    if( (pkt.id != (priorPkt.id + 1) )  &&
         !((pkt.id == 0x0400) && (priorPkt.id == 0x0407)) )
    {
        printf("=== BREAK IN SEQUENCE ===\n");
    }

        // Update prior packet (for debugging)
    priorPkt = pkt;

    printf("SEGWAYIO: pkt: %s\n", pkt.toString());
}
