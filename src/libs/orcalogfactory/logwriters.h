#ifndef ORCALOGFACTORY_LOGWRITERS_H
#define ORCALOGFACTORY_LOGWRITERS_H

#include <orcalog/logwriter.h>
#include <orca/cpu.h>
#include <orca/drivebicycle.h>
#include <orca/imu.h>
#include <orca/laserscanner2d.h>
#include <orca/localise2d.h>
#include <orca/localise3d.h>
#include <orca/odometry2d.h>
#include <orca/odometry3d.h>
#include <orca/polarfeature2d.h>
#include <orca/power.h>
#include <orca/wifi.h>
#include <orca/gps.h>
#include <orca/pathfollower2d.h>

#include <orca/camera.h>
#include <orca/image.h>
#include <orca/multicamera.h>
#include "camerawriter.h"
#include "multicamerawriter.h"

namespace orcalogfactory {

//
// A collection of classes capable of writing sub-logfiles for specific
// interface types.
//

//////////////////////////////////////////////////////////////////////

class CpuLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::CpuData &obj, const orca::Time &arrivalTime );
};

//////////////////////////////////////////////////////////////////////

class DriveBicycleLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::DriveBicycleData &obj, const orca::Time &arrivalTime  );
    void write( const orca::VehicleDescription &descr );
};

//////////////////////////////////////////////////////////////////////

class ImuLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::ImuData &obj, const orca::Time &arrivalTime  );
};

//////////////////////////////////////////////////////////////////////

class LaserScanner2dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::LaserScanner2dDataPtr &obj, const orca::Time &arrivalTime  );
    void write( const orca::RangeScanner2dDescription &descr );
};

//////////////////////////////////////////////////////////////////////

class RangeScanner2dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::RangeScanner2dDataPtr &obj, const orca::Time &arrivalTime  );
    void write( const orca::RangeScanner2dDescription &descr );
};

//////////////////////////////////////////////////////////////////////

class Localise2dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::Localise2dData &obj, const orca::Time &arrivalTime  );
    void write( const orca::VehicleGeometryDescriptionPtr &descr );
};

//////////////////////////////////////////////////////////////////////

class Localise3dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::Localise3dData &obj, const orca::Time &arrivalTime  );        
    void write( const orca::VehicleGeometryDescriptionPtr &descr );
};

//////////////////////////////////////////////////////////////////////

class Odometry2dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::Odometry2dData &obj, const orca::Time &arrivalTime  );
    void write( const orca::VehicleDescription &descr );
};

//////////////////////////////////////////////////////////////////////

class Odometry3dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::Odometry3dData &obj, const orca::Time &arrivalTime  );
    void write( const orca::VehicleDescription &descr );
};

//////////////////////////////////////////////////////////////////////

class PolarFeature2dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::PolarFeature2dData &obj, const orca::Time &arrivalTime  );
};

//////////////////////////////////////////////////////////////////////

class PowerLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::PowerData &obj, const orca::Time &arrivalTime  );
};

//////////////////////////////////////////////////////////////////////

class WifiLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::WifiData &obj, const orca::Time &arrivalTime  );
};

//////////////////////////////////////////////////////////////////////

class GpsLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::GpsData &obj, const orca::Time &arrivalTime  );
    void write( const orca::GpsDescription &descr );
};

//////////////////////////////////////////////////////////////////////

class ImageLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::ImageDataPtr &obj, const orca::Time &arrivalTime  );
    void write( const orca::ImageDescriptionPtr &descr );
};

//////////////////////////////////////////////////////////////////////

class CameraLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::ImageDataPtr &obj, const orca::Time &arrivalTime  );
    void write( const orca::CameraDescriptionPtr &descr );
    void createLogFile( const std::string &filename, const std::string &format );
private:
// alen - member class for camera logging so we can handle jpegs
    CameraWriter cameraWriter;

};

//////////////////////////////////////////////////////////////////////

class MultiCameraLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::MultiCameraDataPtr &obj, const orca::Time &arrivalTime  );
    void write( const orca::MultiCameraDescriptionPtr &descr );
    void initMultiCameraWriter( const orca::MultiCameraDescriptionPtr &descr );
    void createLogFile( const std::string &filename, const std::string &format );
private:
// alen - member class for camera logging so we can handle jpegs
    MultiCameraWriter multiCameraWriter;

};

//////////////////////////////////////////////////////////////////////

class PathFollower2dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::PathFollower2dData &obj, const orca::Time &arrivalTime  );
};

//////////////////////////////////////////////////////////////////////

}

#endif
