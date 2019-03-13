#include "SIM_SerialPort.h"
#include <iostream>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::system;

UartPort::UartPort(const string & port_name) : 
    m_serial_port(NULL), 
    m_port_name(port_name)
{
    m_serial_port = new serial_port(m_ios);
    if(m_serial_port)
    {
        init_port(port_name, 8);
    }
}

UartPort::~UartPort()
{
    if(m_serial_port)
    {
        delete m_serial_port;
    }
}

bool UartPort::init_port(const string& port, const unsigned int char_size)
{
    if (!m_serial_port)
    {
        return false;
    }

    m_serial_port->open(port, m_error_code);

    m_serial_port->set_option(serial_port::baud_rate(9600), m_error_code);
    m_serial_port->set_option(serial_port::flow_control(serial_port::flow_control::none), 
        m_error_code);
    m_serial_port->set_option(serial_port::parity(serial_port::parity::none), m_error_code);
    m_serial_port->set_option(serial_port::stop_bits(serial_port::stop_bits::one), m_error_code);
    m_serial_port->set_option(serial_port::character_size(char_size), m_error_code);

    return true;
}

void  UartPort::Uart_write(const unsigned char * buf, unsigned long len)
{
    if (!m_serial_port)
    {
        return;
    }
    this->m_send_len = len;
    memcpy(this->m_send_buf, buf, len);
    try
    {
        this->m_serial_port->write_some(buffer(this->m_send_buf, this->m_send_len));
        this->m_send_len = 0;
    }
    catch( std::exception& e)
    {
        cout << e.what() << endl;
    }
}

void  UartPort::Uart_read(char * buf, unsigned long len)
{
    async_read(*m_serial_port, 
            buffer(this->m_send_buf, 1), 
            boost::bind(handle_read, 
                this, 
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    memcpy(this->m_send_buf, buf, 1);
}

void UartPort::handle_read(boost::system::error_code ec, size_t bytes_transferred)
{
    cout << "\nhandle_read: \n" ;
}

void UartPort::call_handle()
{
    m_ios.run();
}