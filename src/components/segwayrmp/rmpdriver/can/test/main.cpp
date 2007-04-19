
#include <termios.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>   //read and write to ports
#include <string.h>
#include <iostream>
#include <pthread.h>

#include <libpcan.h>  //stuff to access the peak can drivers

#include <orcaice/context.h>
#include <rmpdriver/rmpio.h>
#include <rmpdriver/canpacket.h>
#include <rmpdriver/can/peakcandriver.h>
#include <rmpdriver/rmpdefs.h>

using namespace std;
using namespace segwayrmp;

void CINToRawMode(void);
void* read_kbd(void*);
volatile double timeout=100;

//******************************************************************
// Very basic stand alone main that purely attempts to 
// read and print CAN packets received from the bus....

int main(void){

    segwayrmp::CanPacket dataMovePacket;           //Store a motion command
    segwayrmp::CanPacket dataPacketCollected;      //Data read from the card


    orcaice::Timer delayTimer;


    std::string portName("/dev/pcan40");

    //declare our Can Driver object
    segwayrmp::PeakCanDriver testCan( portName );

    //And then enable the interface
    testCan.enable(4);
    
    //Simple setup of the can packet to be a slow move forward....
    dataMovePacket.id = RMP_CAN_ID_COMMAND;
    // velocity command does not change any other values
    dataMovePacket.PutSlot(2, (uint16_t)RMP_CMD_NONE);

    // put commands into the packet
    dataMovePacket.PutSlot(0, 0x1BB); //Slow move
    dataMovePacket.PutSlot(1, 0); //Rotation zero
    dataMovePacket.PutSlot(3, 0); //No command param

    

    cerr << "\n\nWarning This code is going to spin the wheels on the segway\n";
    cerr << "Hit return to continue\n";
    cin.get();

    cerr << "Use i / k to increase / kill loop speed\n";
    
    double msecs;

    pthread_t thread_id;
        
    
    //start a new thread here that reads the keyboard.
    pthread_create( &thread_id, NULL, &read_kbd, NULL);



    while(true){ //**??** how should this be escaped?
      //Keep trying to read data from the CAN interface
        if ( testCan.readPacket(&dataPacketCollected) == RmpIo::OK ){ 
            cout<< dataPacketCollected.toString() << endl;
        }

        msecs=delayTimer.elapsed().toMilliSecondsDouble();
        if(msecs >= timeout){
            delayTimer.restart();
            cout << "Sending " << dataMovePacket.toString()<<"delta T  " << msecs <<endl;
            testCan.writePacket(& dataMovePacket);
        }

        

    }
      
  return 0;

}



//*************************************************
// thread that just reads the keyboard input
void* read_kbd(void*){

    CINToRawMode();
    char keystroke;    

    while(true){
        cin>> keystroke;
        switch(tolower(keystroke)){
        case 'i':
            timeout += 2;
            break;
        case 'k':
            timeout -=2;
            break;
        default:
            break;        
        }
            
        cerr << "new timeout " << timeout <<endl;

    }

    return NULL;
}



//***********************************************************
// 
void CINToRawMode(void){
    struct termios raw;
    struct termios cooked;
    int stdin_fd = 0;
   
    // get current (not-raw) console settings
    tcgetattr(stdin_fd , &cooked );
    tcgetattr(stdin_fd , &raw );

    // turn off echo to screen and erase functions
    raw.c_lflag &=~ (ICANON | ECHO);
    raw.c_cc[VEOL] = 1;
    raw.c_cc[VEOF] = 2;

    // put the console in raw mode
    tcsetattr( stdin_fd , TCSANOW, &raw );
    
}