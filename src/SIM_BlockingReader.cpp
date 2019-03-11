#include "SIM_BlockingReader.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::system;

/******************************************************************************/
/**
 * @brief   read_complete
 *
 *              Called when an async read completes or has been cancelled
 *
 * @param[in]   error
 * @param[in]   bytes_transferred
 * @return      None.
 */
/******************************************************************************/
void BlockingReader::read_complete(const error_code& error, size_t bytes_transferred) 
{

    read_error = (error || bytes_transferred == 0);

    // Read has finished, so cancel the timer.
    timer.cancel();
}

// Called when the timer's deadline expires.
/******************************************************************************/
/**
 * @brief   time_out
 *
 *              Called when the timer's deadline expires.
 *
 * @param[in]   error
 * @param[in]   timeout
 * @return      None.
 */
/******************************************************************************/
void BlockingReader::time_out(const error_code& error) 
{
    // Was the timeout cancelled?
    if (error) 
    {
        // yes
        return;
    }

    // no, we have timed out, so kill the read operation
    // The read callback will be called with an error
    port.cancel();
}
/******************************************************************************/
/**
 * @brief   BlockingReader
 *
 *              Constructor
 *
 * @param[in]   port
 * @param[in]   timeout
 * @return      None.
 */
/******************************************************************************/
BlockingReader::BlockingReader(serial_port& port, size_t timeout) :
    port(port),
    timeout(timeout),
    timer(port.get_io_service()),
    read_error(true)
{

}

/******************************************************************************/
/**
 * @brief   read_char
 *
 *              Reads a character or times out returns false if the read times out
 *
 * @param[in]   val
 * @return      None.
 */
/******************************************************************************/
bool BlockingReader::read_char(unsigned char& val) 
{

    val = c = '\0';

    // After a timeout & cancel it seems that we need
    // to do a reset for subsequent reads to work.
    port.get_io_service().reset();

    // Asynchronously read 1 character.
    async_read(port, buffer(&c, 1), bind(&BlockingReader::read_complete,
                    this,
                    placeholders::error,
                    placeholders::bytes_transferred));

    // Setup a deadline time to implement our timeout.
    timer.expires_from_now(posix_time::milliseconds(timeout));
    timer.async_wait(bind(&BlockingReader::time_out, this, placeholders::error));

    // This will block until a character is read
    // or until the it is cancelled.
    port.get_io_service().run();

    if (!read_error)
        val = c;

    return !read_error;
}