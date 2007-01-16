/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Jose Guivant, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER2D_SICK_ACFR_SERIAL_H
#define ORCA2_LASER2D_SICK_ACFR_SERIAL_H

// #ifdef __cplusplus
// extern "C" {
// #endif

namespace laser2d {

class Serial
{
public:

Serial();
~Serial();

// open the serial port
//   name = name of the device (eg. "/dev/ser1")
//   speed0 = baudrate to open at
//   H = handle to the serial port
// int OpenSerialPortXJEG(char *name,int speed0, unsigned *H);
int openSerial( char *name ); 

// read a number of bytes from the serial port
//   h = handle to the serial port
//   buff = buffer to read into
//   lMin = minimum number of bytes to read
//   lMax = maximum number of bytes to read
//   TimeOut = timeout for reading from serial port
int SerialReadJEG( uchar *buff, int count, int TimeOut);
 
// write the buffer to the serial port
//   H = handle to the serial port
//   buff = buffer
int SerialWriteJEG( uchar *buff, int L, int *nw);

// close the serial port
//   H = handle
void CloseSerialPortJEG() ;

// blocks until the next byte which matches "b" arrives
//   h = handle to the serial port
//   b = char to wait for
//   timeoutDs = timeout to wait for "b"
//   ignoredChars = number of bytes ignored
int SerialWaitForCharJEG(unsigned h, uchar b ,int timeoutDs,int *ignoredChars );

// int	ChangeSerialSpeedJEG(unsigned H,int speed);
int	ChangeSerialSpeedJEG( int speed );
  
int SerialReadUntilCharXJEG( uchar *string, int Lmax,uchar b, int timeoutDs, int *ignoredChars);

int CharsWaitingJEG();

int SerialWaitForCharXJEG( uchar b ,int timeoutDs,int *ignoredChars ) ;

char* SpecifySerialPortString(unsigned np);

private:

int port_fd;

int LastSerialSpeedDone;

void InitSerialLib(int *pflagBye);

};

} // namespace

// #ifdef __cplusplus
// }
// #endif

#endif
