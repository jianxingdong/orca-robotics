/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef __CRC32_H
#define __CRC32_H

namespace hydronovatelutil{

int TestASCII(  char* ucBuf_ );
int TestBinary( unsigned char* ucBuf_, int iSize_ );

unsigned long CalculateCRC32(unsigned char* ucBuf_, int iSize_);

}

#endif
