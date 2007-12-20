/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include <stdlib.h>             // for getenv()

#include <orcaobj/stringutils.h>        
#include <hydroutil/stringutils.h>    // for toStringSeq()
#include <hydroutil/mathdefs.h>           // for DEG2RAD_RATIO

#include "proputils.h"
#include "configutils.h"
#include "exceptions.h"
#include "printutils.h"       // initTracerInfo()

using namespace std;

namespace orcaice
{

std::string 
getGlobalConfigFilename( const Ice::PropertiesPtr &props )
{
    std::string filename;

    // first look for Orca.GlobalConfig parameter
    int ret = orcaice::getProperty( props, "Orca.GlobalConfig", filename );
    if ( ret == 0 )
    {
        return filename;
    }

    // now look in this special environment variable
    char *orca_config = getenv("ORCA_GLOBAL_CONFIG");
    if ( orca_config != NULL ) {
        // debug
        cout<<"echo $ORCA_GLOBAL_CONFIG='"<<orca_config<<"'"<<endl;
        // this extra test is mostly because Win32 does not implement
        // unsetenv(). So we have to set the env var to empty string instead.
        if ( strlen(orca_config) ) {
            filename = orca_config;
            return filename;
        }
    }

    // nothing is specified, assume default filename and directory
#ifndef WIN32
    // Linux and friends
    char *home = getenv("HOME");
    if ( home == NULL ) {
        throw hydroutil::Exception( ERROR_INFO, "Environment variable 'HOME' not set, while trying to load .orcarc" );
    }
    
    // start with the home directory
    filename = home;
    // assume the name of the global config file
    filename += "/.orcarc";
#else
    // windows
    filename += "C:\\orca.ini";
#endif
    return filename;
}

std::string 
getApplicationConfigFilename( const Ice::StringSeq & args )
{
    if ( args.empty() ) {
        return std::string( "" );
        // throwing this exception leads to 'unhandled ex' in windows for some reason
        //throw hydroutil::Exception( ERROR_INFO, "Invalid command line options, cannot determine config file name" );
    }

    // first look for Orca.Config parameter
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,13, "--Orca.Config" ) ) {
            return args[i].substr( 14, args[i].size()-1 );
        }
    }

    // now look for Ice.Config parameter (specified by IceGrid)
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,12, "--Ice.Config" ) ) {
            return args[i].substr( 13, args[i].size()-1 );
        }
    }

    // if there's only one argument (other than the program name), 
    // assume it's the path to the config file
    // ... unless it starts with a '-', i.e. looks like an option
    if ( args.size()==2 && args[1].substr(0,1)!="-" ) {
//         cout<<"single argument starts with :"<<args[1].substr(0,1)<<endl;
        return args[1];
    }

    // if there are no arguments (other than the program name),
    // assume a) the config file is in the current directory
    //        b) config file name = executable + '.cfg'
    if ( args.size()==1 ) {
//         cout<<"no arguments"<<endl;
        // strip the path from the executable
#ifndef WIN32
        int lastSlash = args[0].rfind( '/', args[0].size()-1 );
#else
        int lastSlash = args[0].rfind( '\\', args[0].size()-1 );
#endif
        string exeName;
        if ( lastSlash < 0 ) {
            // slash was not found in the executable path
            // not sure why size()-1 clips the last character of the exe name
            exeName = args[0].substr( lastSlash+1, args[0].size() );
        }
        else {
            exeName = args[0].substr( lastSlash+1, args[0].size()-1 );
        }
#ifdef WIN32 
        // This is to get rid of a trailing .exe 
        int exePos = exeName.find(".exe", 0);
        exeName = exeName.substr(0, exePos);
#endif

//         cout << "Exe name is " << exeName << endl;
        return exeName+".cfg";
    }

    // we are here if: there're multiple arguments but none of them are Ice.Config
    return std::string( "" );
}

std::string 
getServiceConfigFilename( const Ice::StringSeq & args )
{
    if ( args.empty() ) {
        throw hydroutil::Exception( ERROR_INFO, "Invalid command line options, cannot determine config file name" );
    }

    // first look for Orca.Config parameter (specified by us manually)
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,13, "--Orca.Config" ) ) {
            return args[i].substr( 14, args[i].size()-1 );
        }
    }

    // now look for Ice.Config parameter (specified by IceGrid)
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,12, "--Ice.Config" ) ) {
            return args[i].substr( 13, args[i].size()-1 );
        }
    }

    // we are here if: there're multiple arguments but none of them are Ice.Config
    return std::string( "" );
}

// note: the standard tracer already exists, if we pass communicator& into here, we could use it here.
int
getProperty( const Ice::PropertiesPtr &prop, const ::std::string& key, std::string &value )
{
    value = prop->getProperty( key );
    if ( value == "" )
    {
        // property not found
        return -1;
    }
    else
    {
        //initTracerInfo( "using configured value '"+value+"' for '"+key+"'");
        return 0;
    }
}

std::string
getPropertyWithDefault( const Ice::PropertiesPtr &prop, const ::std::string& key, const std::string &defaultValue )
{
    std::string value;
    if ( getProperty( prop, key, value ) )
    {
        // use default
        value = defaultValue;

        if ( prop->getPropertyAsIntWithDefault( "Orca.Warn.DefaultProperty", 1 ) ) {
            initTracerInfo( "Set property to default value: "+key+"="+value );
        }
    }
    return value;
}

int
getPropertyAsDouble( const Ice::PropertiesPtr & prop, const ::std::string& key, Ice::Double &value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
    {
        return -1;
    }
    else
    {
        // atof returns 0.0 if failes to convert, can't detect errors
        //d = atof( stringVal.c_str() );
        
        std::istringstream iss( stringVal );
        double d;
        iss >> d;
        if ( !iss ) return -1;
        value = d;
    }
    return 0;
}

Ice::Double
getPropertyAsDoubleWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, Ice::Double defaultValue )
{
    std::stringstream ssDef;
    ssDef << defaultValue;

    std::string stringVal = getPropertyWithDefault( prop, key, ssDef.str() );

    // here it's ok to use atof because we guaranteed to start with a double
    return (Ice::Double)atof( stringVal.c_str() );
}

int
getPropertyAsInt( const Ice::PropertiesPtr & prop, const ::std::string& key, int &value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
    {
        return -1;
    }
    else
    {
        // atoi returns 0 if failes to convert, can't detect errors
        //value = atoi( stringVal.c_str() );
        
        std::istringstream iss( stringVal );
        iss >> value;
        if ( !iss ) return -1;
    }
    return 0;
}

int
getPropertyAsIntWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, int defaultValue )
{
    std::stringstream ssDef;
    ssDef << defaultValue;

    std::string stringVal = getPropertyWithDefault( prop, key, ssDef.str() );
    
    // here it's ok to use atoi because we guaranteed to start with an int
    return atoi( stringVal.c_str() );
}

int
getPropertyAsStringSeq( const Ice::PropertiesPtr & prop, const ::std::string& key, Ice::StringSeq & value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) ) {
        return -1;
    }
    else {
        value = hydroutil::toStringSeq( stringVal, ':' );
        return 0;
    }
}

Ice::StringSeq
getPropertyAsStringSeqWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const Ice::StringSeq & defaultValue )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
        return defaultValue;
    else
        return hydroutil::toStringSeq( stringVal, ':' );
}

int
getPropertyAsFrame2d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Frame2d & value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toFrame2d( stringVal, value );
}

orca::Frame2d
getPropertyAsFrame2dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Frame2d & defaultValue )
{
    orca::Frame2d value;
    if ( getPropertyAsFrame2d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsFrame3d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Frame3d & value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toFrame3d( stringVal, value );
}

orca::Frame3d
getPropertyAsFrame3dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Frame3d & defaultValue )
{
    orca::Frame3d value;
    if ( getPropertyAsFrame3d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsSize2d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Size2d & value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toSize2d( stringVal, value );
}

orca::Size2d
getPropertyAsSize2dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Size2d & defaultValue )
{
    orca::Size2d value;
    if ( getPropertyAsSize2d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsSize3d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Size3d & value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toSize3d( stringVal, value );
    return 0;
}

orca::Size3d
getPropertyAsSize3dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Size3d & defaultValue )
{
    orca::Size3d value;
    if ( getPropertyAsSize3d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsCartesianPoint( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::CartesianPoint & value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toCartesianPoint( stringVal, value );
    return 0;
}

orca::CartesianPoint
getPropertyAsCartesianPointWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::CartesianPoint & defaultValue )
{
    orca::CartesianPoint value;
    if ( getPropertyAsCartesianPoint( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsIntVector( const Ice::PropertiesPtr & prop, const ::std::string& key, std::vector<int> & value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toIntVector( stringVal, value );
    return 0;
}

std::vector<int>
getPropertyAsIntVectorWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const std::vector<int>& defaultValue )
{
    std::vector<int> value;
    if ( getPropertyAsIntVector( prop, key, value ) )
    {
        std::ostringstream s;
        for ( unsigned int i=0; i<defaultValue.size(); i++ )
        {
            s << defaultValue.at(i) << " ";
        }
        
        initTracerInfo( "Set property to default value: "+key+"="+s.str() );
        
        return defaultValue;
    }
    else
    {
        return value;
    }
}

int
getPropertyAsDoubleVector( const Ice::PropertiesPtr & prop, const ::std::string& key, std::vector<double> & value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
    {
        return -1;
    }
    else
    {
        return toDoubleVector( stringVal, value );
    }
    return 0;
}

std::vector<double>
getPropertyAsDoubleVectorWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const std::vector<double>& defaultValue )
{
    std::vector<double> value;
    if ( getPropertyAsDoubleVector( prop, key, value ) )
    {
        std::ostringstream s;
        for ( unsigned int i=0; i<defaultValue.size(); i++ )
        {
            s << defaultValue.at(i) << " ";
        }
        
        initTracerInfo( "Set property to default value: "+key+"="+s.str() );
        return defaultValue;
    }
    else
    {
        return value;
    }
}

int 
getPropertyAsTimeDuration( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Time &value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toTimeDuration( stringVal, value );
}

orca::Time
getPropertyAsTimeDurationWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Time & defaultValue )
{
    orca::Time value;
    if ( getPropertyAsTimeDuration( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

std::string 
warnMissingProperty( const std::string & prop )
{
    return "Property '" + prop + "' is not set.";
}

std::string 
warnMissingPropertyWithDefault( const std::string & prop, const std::string & defaultValue )
{
    return warnMissingProperty(prop) + " Using default value '" + defaultValue + "'.";
}

std::string 
warnMissingProperty( const std::string & info, const std::string & prop )
{
    return "Missing " + info + " because property '" + prop + "' is not set.";
}

std::string 
warnMissingPropertyWithDefault( const std::string & info, const std::string & prop, const std::string & defaultValue )
{
    return warnMissingProperty(info,prop) + " Using default value '" + defaultValue + "'.";
}

} // namespace
