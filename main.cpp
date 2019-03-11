#include <iostream>
#include <boost/asio.hpp>

using namespace std;

int main(void)
{
    
    try 
    {
        boost::asio::io_service io;
        boost::asio::serial_port sp(io,"COM2");

        //设置串口参数
        sp.set_option(boost::asio::serial_port::baud_rate(9600));
        sp.set_option(boost::asio::serial_port::flow_control());
        sp.set_option(boost::asio::serial_port::parity());
        sp.set_option(boost::asio::serial_port::stop_bits());
        sp.set_option(boost::asio::serial_port::character_size(8));

        boost::system::error_code err;
        while(true)
        {
            char buf[]="hello";
            int nWrite = sp.write_some(boost::asio::buffer(buf), err);
            if(err)
            {
                cout << "write_some err,errmessage:" << err.message() << endl;
                break;
            }
            else
            {
                char buf[1024];
                sp.read_some(boost::asio::buffer(buf), err);
                if(!err)
                {
                    cout << "recv data:" << buf << endl;
                }
            }
        }
        io.run();
    }
    catch (exception& e)
    {
        cout << "Exceptions Error: " << e.what() << endl;
    }   
    return 0;
}


