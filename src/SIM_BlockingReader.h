#ifndef __SIM_BLOCKING_READER_h__
#define __SIM_BLOCKING_READER_h__

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/bind.hpp>

using namespace boost::asio;
using namespace boost::system;

class BlockingReader
{
private:
    serial_port&            port;
    size_t                  timeout;
    unsigned char           c;
    deadline_timer          timer;
    bool                    read_error;

private:
    void            read_complete(const error_code& error, size_t bytes_transferred);   // Called when an async read completes or has been cancelled
    void            time_out(const error_code& error);      // called when the timer's deadline expires

public:
    // Constructs a blocking reader, pass in an open serial_port and
    // a timeout in milliseconds.
    BlockingReader(serial_port& port, size_t timeout);
    
    // Reads a character or times out
    // returns false if the read times out
    bool read_char(unsigned char& val);
};

#endif /* __SIM_BLOCKING_READER_h__ */
