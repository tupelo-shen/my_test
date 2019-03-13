#ifndef __SIM_SERIALPORT_H__
#define __SIM_SERIALPORT_H__

#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;

static const int MAX_FIFO_SIZE = 512;

class UartPort
{
public:
    UartPort(const string & port_name);
    ~UartPort();

private:
    bool                init_port(const string& port, const unsigned int char_size);

public:
    void                Uart_write(const unsigned char * buf, unsigned long len);
    void                Uart_read(char * buf, unsigned long len);
    void                handle_read(boost::system::error_code ec, size_t bytes_transferred);
    void                call_handle();

private:
    io_service                  m_ios;
    serial_port*                m_serial_port;
    string                      m_port_name;
    boost::system::error_code   m_error_code;
    unsigned char               m_send_buf[MAX_FIFO_SIZE];
    unsigned long               m_send_len = 0;            
};

#endif /* __SIM_SERIALPORT_H__ */
