#ifndef __SIM_UARTTIMERSIM_H__
#define __SIM_UARTTIMERSIM_H__

#include <iostream>
#include <functional>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost;
using namespace boost::asio;
using namespace boost::system;
/*
 * Uart Timer Simulator Class<br>
 */
class UartTimerSim
{
public:
    static const unsigned short TRG_START = 0x0400;
    static const unsigned short TRG_TFRMEND = 0x0200;
    static const unsigned short TRG_RXGAP = 0x0100;

    typedef std::function<void(const error_code &ec)> handler_func_ptr_t;

    /**
     * Constructor
     */
    UartTimerSim();
    /**
     * Destructor
     */
    ~UartTimerSim();

    void init(handler_func_ptr_t pHandler);
    bool startTimer(unsigned short trigger);
    void cancelTimer();
    void setEnable(int enable);
    void setTrigger(unsigned short trigger);
    void setTimeout(unsigned int timeout)
    {
        this->timeout = timeout;
    }

private:
    deadline_timer*         pTimer;
    thread*                 pThread;
    io_service              io_service;

    handler_func_ptr_t      pHandler;

    unsigned int            timeout;
    unsigned int            enable;
    unsigned short          trigger;

    void startTimer();
    void runTimerThread();
};

#endif /* __SIM_UARTTIMERSIM_H__ */