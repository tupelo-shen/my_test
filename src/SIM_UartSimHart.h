#ifndef SOURCE_SIMULATOR_UARTSIMHART_H_
#define SOURCE_SIMULATOR_UARTSIMHART_H_

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/lock_guard.hpp>

#include "PF_UART_Driver.h"
#include "SIM_BlockingReader.h"
#include "SIM_UartTimerSim.h"
#include "SIM_UartSim.h"

using namespace boost::asio;


class UartSimHart : public UartSim
{
private:
    virtual void run();

public:
    UartSimHart();
    virtual ~UartSimHart();
    virtual unsigned long recvPacket(unsigned char* pBuf, unsigned long buf_size);
    virtual int US_SetTimer(pf_uart_timer_t timer, unsigned int val);
};

#endif // SOURCE_SIMULATOR_UARTSIMHART_H_
