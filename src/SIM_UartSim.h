#ifndef __SIM_UARTSIM_H__
#define __SIM_UARTSIM_H__

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/lock_guard.hpp>

#include "PF_UART_Driver.h"
#include "SIM_UartTimerSim.h"
#include "SIM_BlockingReader.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::system;

static const int MAX_FIFO_SIZE = 512;

class UartSim
{
protected:
    io_service          m_io;
    serial_port*        m_port;
    BlockingReader*     m_pReader;
    const char*         m_portname;

    unsigned int        m_RE;                       // recieve enable
    unsigned int        m_TE;                       // transmit enable
    unsigned int        m_TTRG;
    unsigned int        m_RTRG;
    unsigned int        m_baudrate;

    unsigned char       m_send_buf[MAX_FIFO_SIZE];
    unsigned long       m_send_len = 0;
    thread              m_thr_rcv;
    UartTimerSim        m_timer0;
    UartTimerSim        m_timer1;
    
    enum state
    {
        HAS_WORK,
        NO_WORK,
        SHUTDOWN
    };

    state               thread_state;
    mutex               thread_mutex;
    condition_variable  thread_condition;

protected:
    void                getParity(pf_uart_parity_t p);
    int                 getBaudRate(pf_uart_baudrate_t b);
    int                 getDataBit(pf_uart_databit_t b);
    virtual void        run();
    void                HandlerRecvTimer0(const error_code &ec);
    void                HandlerRecvTimer1(const error_code &ec);
    unsigned long       getFIFOTrg();

public:
    UartSim();
    virtual ~UartSim();

public:
    PF_UART_Event_t     m_evt_func;
    void*               userData;
    unsigned char       m_recv_buf[MAX_FIFO_SIZE];
    unsigned long       m_recv_len;    

    
    const char*             getPortName(){ return m_portname; };
    virtual unsigned long   recvPacket(unsigned char* pBuf, unsigned long buf_size);
    void                    US_Open(const char * comname, int openmode);
    int                     US_ioctl(const char *name, 
                                    const pf_uart_config_t *config, 
                                    PF_UART_Event_t cb_event, 
                                    void* userData);
    int                     US_write(FILE_HANDLE fh, 
                                    const unsigned char * buf, 
                                    unsigned long len, 
                                    int mode);
    int                     US_read(FILE_HANDLE fh, 
                                    unsigned char * buf, 
                                    unsigned long len, 
                                    int mode);
    void                    US_close();
    int                     US_SetTransmitFifoSize(unsigned int val);
    int                     US_SetReceiveFifoSize(unsigned int val);
    int                     US_GetReceiveFifoTriggerSize();
    int                     US_SetTransmitEnable(unsigned int val);

    int                     US_TimerStart(pf_uart_timer_t timer, unsigned int val);
    int                     US_TransmitCompleteTimerStart(pf_uart_timer_t timer, unsigned int val);
    int                     US_ReceiveGapTimerStart(pf_uart_timer_t timer, unsigned int val);

    virtual int             US_SetTimer(pf_uart_timer_t timer, unsigned int val);
    int                     US_SetTimerEnable(pf_uart_timer_t timer, unsigned int val);
    int                     US_SetReceiveEnable(unsigned int val);
    int                     US_ClearReceiveFifo(unsigned int val);
    int                     US_SetTransferNumber(unsigned int val);
};

#endif /* __SIM_UARTSIM_H__ */
