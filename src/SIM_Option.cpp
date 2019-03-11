
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "SIM_Option.h"

using namespace boost;
using namespace boost::property_tree;

Options * Options::obj = 0;

// -----<< class Options >>-----
Options::Options( const char * fname )
{
    // -----< ini file name >-----
    Options::iniFileName = fname;
    // -----< load ini file >-----
    ptree pt;
    read_ini( fname, pt );
    for( unsigned char idx = 0; idx < 5; idx ++ )
    {
        static const char * keys[5] = { "COM.Sensor1", "COM.Sensor2", "COM.Hart", "COM.RS485", "COM.Bluetooth" };
        if( boost::optional<string> str = pt.get_optional<string>( keys[idx] ) )
        {
            uart[idx] = str.get();
        }
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Flash.Program" ) )
    {
        progtamFlash = str.get();
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Flash.UserData" ) )
    {
      dataFlash = str.get();
    }
    logFileName = "";
    if( boost::optional<string> str = pt.get_optional<string>( "Log.LogFile" ) )
    {
        logFileName = str.get();
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Log.DisplayLog" ) )
    {
        logSwitch = str.get();
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Log.DisplayImageFolder" ) )
    {
        logFolder = str.get();
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Sensor.ch1" ) )
    {
        sensor_ch[0] = str.get();
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Sensor.ch2" ) )
    {
        sensor_ch[1] = str.get();
    }
    obj = this;
}

Options::~Options(void)
{
    obj = 0;
}

Options & Options::refObject(void)
{
    assert( 0 != obj );
    return *obj;
}