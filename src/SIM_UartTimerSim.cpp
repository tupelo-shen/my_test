#include <boost/date_time/posix_time/posix_time_io.hpp>
#include "SIM_UartTimerSim.h"

/**
 * Constructor
 */
UartTimerSim::UartTimerSim() :
    pThread(NULL), 
    timeout(0), 
    enable(0), 
    trigger(0)
{
    this->pTimer = new boost::asio::deadline_timer(this->io_service);

    if (this->pThread == NULL)
    {
        this->pThread = new boost::thread(&UartTimerSim::runTimerThread, this);
    }
}

/**
 * Destructor
 */
UartTimerSim::~UartTimerSim()
{
    this->io_service.stop();
    this->pThread->join();
}

/**
 * Initialize
 *
 * @param pHandler Handler
 */
void UartTimerSim::init(handler_func_ptr_t pHandler)
{
    this->pHandler = std::move(pHandler);
}

/**
 * Start timer
 *
 * @param trigger Trigger
 * @return result (if timer started, then return TRUE)
 */
bool UartTimerSim::startTimer(unsigned short trigger)
{
    bool ret = false;
    if (this->trigger & trigger)
    {
        startTimer();
        ret = true;
    }
    return ret;
}

/**
 * Set enable/disable
 *
 * @param enable Enable/Disable
 */
void UartTimerSim::setEnable(int enable)
{
    this->enable = enable;

    if (this->enable != 1)
    {
        cancelTimer();
    } else
    {
        if (this->trigger & TRG_START)
        {
            startTimer();
        }
    }
}

/**
 * Set enable/disable
 *
 * @param enable Enable/Disable
 */
void UartTimerSim::setTrigger(unsigned short trigger)
{
    if (trigger & TRG_START)
    {
        this->trigger |= trigger;
    } else
    {
        this->trigger = trigger;
    }

    if ((this->enable == 1) && (this->trigger & TRG_START))
    {
        startTimer();
    }
}

/**
 * Start timer
 */
void UartTimerSim::startTimer()
{
    this->pTimer->expires_from_now(boost::posix_time::milliseconds(this->timeout));
    this->pTimer->async_wait(this->pHandler);

    this->trigger &= ~TRG_START;
}

/**
 * Cancel timer
 */
void UartTimerSim::cancelTimer()
{
    this->pTimer->cancel();
}

/**
 * Timer thread
 */
void UartTimerSim::runTimerThread()
{
    boost::asio::io_service::work w(this->io_service);
    this->io_service.run();
}
