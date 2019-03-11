#ifndef __SIM_OPTION_H__
#define __SIM_OPTION_H__

#include <string>

using namespace std;

class Options
{
private:
    static Options * obj;
public:
    string  iniFileName;
    string  uart[5];
    string  logSwitch;
    string  logFolder;
    string  logFileName;
    string  progtamFlash;
    string  dataFlash;
    string  sensor_ch[2];

    Options( const char * fname );
    virtual ~Options( void );
    static Options & refObject(void );
};

#endif /* __SIM_OPTION_H__ */
