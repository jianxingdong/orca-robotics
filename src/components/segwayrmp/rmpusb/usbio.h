/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_SEGWAY_RMP_USB_IO_H
#define ORCA2_SEGWAY_RMP_USB_IO_H

#include "canpacket.h"

class UsbIo
{
public:    
    virtual int init()=0;
    virtual int shutdown()=0;

    /*!
     * Blocks until a packet is available.
     * Returns 0 if copied a packet, non-zero on error.
     */
    virtual int readPacket(CanPacket* pkt)=0;
    
    /*!
     * Returns: 0 on success, non-zero error code otherwise.
     */
    virtual int writePacket(CanPacket* pkt)=0;
    
};

#endif
