#ifndef SEGWAYRMPDRIVERTHREAD_STALLSENSOR_H
#define SEGWAYRMPDRIVERTHREAD_STALLSENSOR_H

#include <gbxsickacfr/gbxiceutilacfr/timer.h>
#include <hydrointerfaces/segwayrmp.h>
#include <iostream>

namespace hydrormputil {

// Tells us what kind of stall-situation we have
enum StallType {
    NoStall,
    BothMotorsStall,
    LeftMotorStall,
    RightMotorStall,
};
std::string toString( StallType t );
inline std::ostream &operator<<( std::ostream &s, StallType t )
{ return s << toString(t); }

//
// @brief Looks for high motor torques with no motion
//
// @author Alex Brooks
//
class StallSensor
{
public: 

    class Config {
    public:
        // Trigger an alert when the torque is higher than ...
        double torqueThreshold;
        // ... but the speed is less than ...
        double speedThreshold;
        // ... for a period of more than ...
        double timeThreshold;
    };

    StallSensor( const Config &config )
        : timerIsRunning_(false),
          config_(config)
        {}

    StallType isStalled( const hydrointerfaces::SegwayRmp::Data &data );

private: 

    gbxiceutilacfr::Timer stalledTimer_;
    bool timerIsRunning_;

    const Config config_;

};

}

#endif
