#include <iostream>
#include <string>
#include "SIM_UartSim.h"
#include "SIM_Option.h"

#define MAX_SERIAL_PORT             (5)         // Sensor module 1/2, hart, rs485, bluetooth


static const char* PORT_NAME[MAX_SERIAL_PORT] =
{
    ":UART0",    // UART_TYPE_SENSOR_MODULE_1
    ":UART1",    // UART_TYPE_SENSOR_MODULE_2
    ":UART2",    // UART_TYPE_HART
    ":UART3",    // UART_TYPE_RS485
    ":UART4",    // UART_TYPE_BLUETOOTH
};

/******************************************************************************/
/**
 * @brief   getParity
 *
 *              
 *
 * @param[in]   p
 * @return      None
 */
/******************************************************************************/
void UartSim::getParity(pf_uart_parity_t p)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::getParity(): " << this->m_portname << std::endl;
#endif

    switch (p)
    {
    case pf_parity_odd:
        m_port->set_option(serial_port_base::parity(serial_port_base::parity::odd));
        break;
    case pf_parity_even:
        m_port->set_option(serial_port_base::parity(serial_port_base::parity::even));
        break;
    case pf_parity_none:
    default:
        m_port->set_option(serial_port_base::parity(serial_port_base::parity::none));
        break;
    }
}

/******************************************************************************/
/**
 * @brief   getFIFOTrg
 *
 *              recieve thread
 *
 * @param[in]   None
 * @return      None
 */
/******************************************************************************/
int UartSim::getBaudRate(pf_uart_baudrate_t b)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::getBaudRate(): " << this->m_portname << std::endl;
#endif

    int baudrate = 0;
    switch (b)
    {
    case pf_baudrate_1200:
        baudrate = 1200;
        break;
    case pf_baudrate_4800:
        baudrate = 4800;
        break;
    case pf_baudrate_19200:
        baudrate = 19200;
        break;
    case pf_baudrate_38400:
        baudrate = 38400;
        break;
    case pf_baudrate_115200:
        baudrate = 115200;
        break;
    case pf_baudrate_9600:
    default:
        baudrate = 9600;
        break;
    }

    return baudrate;
}

/******************************************************************************/
/**
 * @brief   getDataBit
 *
 *              
 *
 * @param[in]   b
 * @return      None
 */
/******************************************************************************/
int UartSim::getDataBit(pf_uart_databit_t b)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::getDataBit(): " << this->m_portname << std::endl;
#endif

    switch (b)
    {
    case pf_databit_7: /**< 7 bit */
        return 7;
    case pf_databit_8: /**< 8 bit */
    default:
        return 8;
    }
}

/******************************************************************************/
/**
 * @brief   HandlerRecvTimer0
 *
 *              
 *
 * @param[in]   None
 * @return      None
 */
/******************************************************************************/
void UartSim::HandlerRecvTimer0(const boost::system::error_code &ec)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::HandlerRecvTimer0(): " << this->m_portname << std::endl;
#endif

    if (ec == boost::asio::error::operation_aborted)
    {
//        ModbusUtil::trace("HandlerRecvTimer(): cancelled or retriggered");
        return;
    }

    if (this->m_evt_func != NULL)
    {
        this->m_evt_func(PF_UART_EVENT_RX_TIMER0_TIMEOUT, this->userData);
    }
}

/******************************************************************************/
/**
 * @brief   HandlerRecvTimer1
 *
 *              
 *
 * @param[in]   None
 * @return      None
 */
/******************************************************************************/
void UartSim::HandlerRecvTimer1(const boost::system::error_code &ec)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::HandlerRecvTimer1(): " << this->m_portname << std::endl;
#endif

    if (ec == boost::asio::error::operation_aborted)
    {
//        ModbusUtil::trace("HandlerRecvTimer(): cancelled or retriggered");
        return;
    }

    if (this->m_evt_func != NULL)
    {
        this->m_evt_func(PF_UART_EVENT_RX_TIMER1_TIMEOUT, this->userData);
    }
}

/******************************************************************************/
/**
 * @brief   getFIFOTrg
 *
 *              recieve thread
 *
 * @param[in]   None
 * @return      None
 */
/******************************************************************************/
unsigned long UartSim::getFIFOTrg()
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::getFIFOTrg(): " << this->m_portname << std::endl;
#endif

    unsigned long recvFifoTrgDataCnt;

    switch (m_RTRG)
    {
    default:
    case 0x00:
        recvFifoTrgDataCnt = 1;
        break;
    case 0x01:
        recvFifoTrgDataCnt = 128;
        break;
    case 0x10:
        recvFifoTrgDataCnt = 256;
        break;
    case 0x11:
        recvFifoTrgDataCnt = 384;
        break;
    }

    return recvFifoTrgDataCnt;
}

/******************************************************************************/
/**
 * @brief   contructor
 *
 *              contructor
 *
 * @param[in]   None
 * @return      None
 */
/******************************************************************************/
UartSim::UartSim()
{
    this->thread_state = NO_WORK;

    this->m_thr_rcv = boost::thread(&UartSim::run, this);
    this->m_timer0.init(std::bind(&UartSim::HandlerRecvTimer0, this, std::placeholders::_1));
    this->m_timer1.init(std::bind(&UartSim::HandlerRecvTimer1, this, std::placeholders::_1));
}

/******************************************************************************/
/**
 * @brief   ~UartSim
 *
 *              destructor
 *
 * @param[in]   None
 * @return      None
 */
/******************************************************************************/
UartSim::~UartSim()
{
//    ModbusUtil::trace("UartSim::~UartSim()");
    this->m_thr_rcv.interrupt();
    this->m_thr_rcv.join();
}

/******************************************************************************/
/**
 * @brief   recvPacket
 *
 *              recieve thread
 *
 * @param[in]   None
 * @return      None
 */
/******************************************************************************/
unsigned long UartSim::recvPacket(unsigned char* pBuf, unsigned long buf_size)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::recvPacket(): " << this->m_portname << std::endl;
#endif

    // データの受信
    unsigned long recv_len = 0; // return value: receive byte size

    while (recv_len == 0 && this->thread_state == NO_WORK)
    {
        unsigned char c;

        if (this->m_pReader == 0x0)
        {
            return 0;
        }

        // 1バイトづつ受信し、TIMEOUTが発生した時点で受信処理を完了
        while (this->m_pReader->read_char(c) && recv_len < buf_size)
        {
            pBuf[recv_len] = c;
            recv_len++;

            if (recv_len == getFIFOTrg())
            {
                // call interrupt routine
                if (this->m_evt_func != NULL)
                {
                    this->m_evt_func(PF_UART_EVENT_RX_FIFO_FULL, this->userData);
                }
                this->m_timer0.cancelTimer();
                this->m_timer1.cancelTimer();
            }
        }
    }

    return recv_len;
}

/******************************************************************************/
/**
 * @brief   run
 *
 *              recieve thread
 *
 * @param[in]   None
 * @return      None
 */
/******************************************************************************/
void UartSim::run()
{
    for (;;)
    {
        {
            boost::unique_lock<boost::mutex> lock(this->thread_mutex);

            while (NO_WORK == this->thread_state)
            {
                this->thread_condition.wait(lock);
            }

            if (SHUTDOWN == this->thread_state)
                break;

            this->thread_state = NO_WORK;
        }

        // do some work here
        {
            this->m_recv_len = recvPacket(this->m_recv_buf, sizeof(this->m_recv_buf));
        }

        if (this->m_RE != 0 && this->m_evt_func != NULL && this->m_recv_len != 0)
        {
            this->m_evt_func(PF_UART_EVENT_RX_TIMER0_TIMEOUT, this->userData);
            this->m_evt_func(PF_UART_EVENT_RX_TIMER1_TIMEOUT, this->userData);
        }
    }
}

/******************************************************************************/
/**
 * @brief   US_Open
 *
 *              
 *
 * @param[in]   comname
 * @return      openmode
 */
/******************************************************************************/
void UartSim::US_Open(const char * comname, int openmode)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_Open(): " << (char*)comname << std::endl;
#endif

    int port_idx = 0;
    for ( ; port_idx < MAX_SERIAL_PORT; port_idx++)
    {
        std::string com(comname);
        if (com == PORT_NAME[port_idx])
        {
            break;
        }
    }

    Options & opt = Options::refObject();

    try
    {
        this->m_port = new serial_port(m_io);

        if (this->m_port)
        {
            this->m_port->open(opt.uart[port_idx]);
            this->m_portname = PORT_NAME[port_idx];
            this->m_pReader = new BlockingReader(*this->m_port, 100 /*interval_timeout*/);
        }
    } catch (const std::exception& exp)
    {
        cout << exp.what() << endl;
    }
}

/******************************************************************************/
/**
 * @brief   US_Open
 *
 *              
 *
 * @param[in]   comname
 * @return      openmode
 */
/******************************************************************************/
int UartSim::US_ioctl(
        const char *name, 
        const pf_uart_config_t *config, 
        PF_UART_Event_t cb_event, 
        void* userData)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_ioctl(): " << this->m_portname << std::endl;
#endif

//    ModbusUtil::trace("US_ioctl");

    this->m_evt_func = cb_event;
    this->userData = userData;

    this->m_baudrate = getBaudRate(config->baudrate);
    m_port->set_option(serial_port_base::baud_rate(this->m_baudrate));
    getParity(config->parity);
    m_port->set_option(serial_port_base::character_size(getDataBit(config->databit)));

    switch (config->stopbit)
    {
    case pf_stopbit_1: /**< 1 bit */
        m_port->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
        break;
    case pf_stopbit_2: /**< 2 bit */
        m_port->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::two));
        break;
    default:
        break;
    }

    switch (config->flowcontrol)
    {
    case pf_flowcontrol_none:
    default:
        m_port->set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
        break;
    case pf_flowcontrol_rts:
        m_port->set_option(serial_port_base::flow_control(serial_port_base::flow_control::hardware));
        break;
    }

    return 0;
}

/******************************************************************************/
/**
 * @brief   US_write
 *
 *              
 *
 * @param[in]   fh
 * @return      mode
 */
/******************************************************************************/
int UartSim::US_write(FILE_HANDLE fh, const unsigned char * buf, unsigned long len, int mode)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_write(): " << this->m_portname << std::endl;
#endif
    //send data
    this->m_send_len = len;
    memcpy(this->m_send_buf, buf, len);

    // 送信開始条件チェック
    if ((this->m_TE != 0) && (this->m_send_len != 0))
    {
        try
        {
            this->m_port->write_some(buffer(this->m_send_buf, this->m_send_len));
            this->m_send_len = 0;   // clear length
        } catch (const std::exception& exp)
        {
            cout << exp.what() << endl;
        }

        if (this->m_evt_func != NULL)
        {
            this->m_evt_func(PF_UART_EVENT_TX_FRAME_COMPLETE, this->userData);
        }
        this->m_timer0.startTimer(UartTimerSim::TRG_TFRMEND);
        this->m_timer1.startTimer(UartTimerSim::TRG_TFRMEND);
    }
    return 0;
}

/******************************************************************************/
/**
 * @brief   US_read
 *
 *              
 *
 * @param[in]   comname
 * @return      openmode
 */
/******************************************************************************/
int UartSim::US_read(FILE_HANDLE fh, unsigned char * buf, unsigned long len, int mode)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_read(): " << this->m_portname << std::endl;
#endif

    memcpy(buf, m_recv_buf, m_recv_len);
    m_recv_len = 0; // clear length

    return 0;
}

/******************************************************************************/
/**
 * @brief   US_SetTransmitFifoSize
 *
 *              
 *
 * @param[in]   val
 * @return      openmode
 */
/******************************************************************************/
void UartSim::US_close()
{
    this->m_evt_func = NULL;
    this->m_timer0.cancelTimer();
    this->m_timer1.cancelTimer();

    {
        boost::lock_guard<boost::mutex> lock(this->thread_mutex);

        this->thread_state = SHUTDOWN;
        this->thread_condition.notify_one();
    }
}

/******************************************************************************/
/**
 * @brief   US_SetTransmitFifoSize
 *
 *              
 *
 * @param[in]   val
 * @return      None
 */
/******************************************************************************/
int UartSim::US_SetTransmitFifoSize(unsigned int val)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_SetTransmitFifoSize(" << val << "): " << this->m_portname << std::endl;
#endif

    this->m_TTRG = val;
    return 0;
}

/******************************************************************************/
/**
 * @brief   US_SetReceiveFifoSize
 *
 *              
 *
 * @param[in]   val
 * @return      None
 */
/******************************************************************************/
int UartSim::US_SetReceiveFifoSize(unsigned int val)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_SetReceiveFifoSize(" << val << "): " << this->m_portname << std::endl;
#endif

    this->m_RTRG = val;
    return 0;
}

/******************************************************************************/
/**
 * @brief   US_GetReceiveFifoTriggerSize
 *
 *              
 *
 * @param[in]   None
 * @return      None
 */
/******************************************************************************/
int UartSim::US_GetReceiveFifoTriggerSize()
{
    return this->m_RTRG;
}

/******************************************************************************/
/**
 * @brief   US_SetTransmitEnable
 *
 *              
 *
 * @param[in]   val
 * @return      None
 */
/******************************************************************************/
int UartSim::US_SetTransmitEnable(unsigned int val)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_SetTransmitEnable(" << val << "): " << this->m_portname << std::endl;
#endif

    this->m_TE = val;

    // 送信開始条件チェック
    if ((this->m_TE != 0) && (this->m_send_len != 0))
    {
        try
        {
            this->m_port->write_some(buffer(this->m_send_buf, this->m_send_len));
            this->m_send_len = 0;   // clear length
        } catch (const std::exception& exp)
        {
            cout << exp.what() << endl;
        }

        if (this->m_evt_func != NULL)
        {
            this->m_evt_func(PF_UART_EVENT_TX_FRAME_COMPLETE, this->userData);
        }
        this->m_timer0.startTimer(UartTimerSim::TRG_TFRMEND);
        this->m_timer1.startTimer(UartTimerSim::TRG_TFRMEND);
    }
    return 0;
}

/******************************************************************************/
/**
 * @brief   US_TimerStart
 *
 *              
 *
 * @param[in]   timer
 * @param[in]   val
 * @return      None
 */
/******************************************************************************/
int UartSim::US_TimerStart(pf_uart_timer_t timer, unsigned int val)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_Timer0Start(" << val << "): " << this->m_portname << std::endl;
#endif

    switch (timer)
    {
    case pf_uart_timer_0:
        this->m_timer0.setTrigger(UartTimerSim::TRG_START);
        break;

    case pf_uart_timer_1:
        this->m_timer1.setTrigger(UartTimerSim::TRG_START);
        break;

    default:
        break;
    }

    return 0;
}

/******************************************************************************/
/**
 * @brief   US_TransmitCompleteTimerStart
 *
 *              
 *
 * @param[in]   timer
 * @param[in]   val
 * @return      None
 */
/******************************************************************************/
int UartSim::US_TransmitCompleteTimerStart(pf_uart_timer_t timer, unsigned int val)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_TransmitCompleteTimerStart(" << val << "): " << this->m_portname << std::endl;
#endif

    switch (timer)
    {
    case pf_uart_timer_0:
        this->m_timer0.setTrigger(UartTimerSim::TRG_TFRMEND);
        break;

    case pf_uart_timer_1:
        this->m_timer1.setTrigger(UartTimerSim::TRG_TFRMEND);
        break;

    default:
        break;
    }

    return 0;
}

/******************************************************************************/
/**
 * @brief   US_ReceiveGapTimerStart
 *
 *              
 *
 * @param[in]   timer
 * @param[in]   val
 * @return      None
 */
/******************************************************************************/
int UartSim::US_ReceiveGapTimerStart(pf_uart_timer_t timer, unsigned int val)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_ReceiveGapTimerStart(" << val << "): " << this->m_portname << std::endl;
#endif

    switch (timer)
    {
    case pf_uart_timer_0:
        this->m_timer0.setTrigger(UartTimerSim::TRG_RXGAP);
        break;

    case pf_uart_timer_1:
        this->m_timer1.setTrigger(UartTimerSim::TRG_RXGAP);
        break;

    default:
        break;
    }

    return 0;
}

/******************************************************************************/
/**
 * @brief   US_SetTimer
 *
 *              
 *
 * @param[in]   timer
 * @param[in]   val
 * @return      None
 */
/******************************************************************************/
int UartSim::US_SetTimer(pf_uart_timer_t timer, unsigned int val)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_SetTimer0(" << val << "): " << this->m_portname << std::endl;
#endif

    if (val < 300)  // RXGAP 1.5 Character = 289
    {
        val = 2;    // 約1.7ms
    }

    if (val < 700)  // RXGAP 3.5 Character = 641
    {
        val = 4;    // 約3.99ms
    } else          // TOUT VAL 100ms = 15368 (baudrate: 9600)
    {
        val = 100;  // 100ms
    }

    switch (timer)
    {
    case pf_uart_timer_0:
        this->m_timer0.setTimeout(val);
        break;

    case pf_uart_timer_1:
        this->m_timer1.setTimeout(val);
        break;

    default:
        break;
    }

    return 0;
}

/******************************************************************************/
/**
 * @brief   US_SetTransferNumber
 *
 *              
 *
 * @param[in]   val
 * @return      None
 */
/******************************************************************************/
int UartSim::US_SetTransferNumber(unsigned int val)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_SetTransferNumber(" << val << "): " << this->m_portname << std::endl;
#endif

    // TODO TXD_NUM
    return 0;
}

/******************************************************************************/
/**
 * @brief   US_SetTransferNumber
 *
 *              
 *
 * @param[in]   val
 * @return      None
 */
/******************************************************************************/
int UartSim::US_SetTimerEnable(pf_uart_timer_t timer, unsigned int val)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_SetTimer0Enable(" << val << "): " << this->m_portname << std::endl;
#endif

    switch (timer)
    {
    case pf_uart_timer_0:
        this->m_timer0.setEnable(val);
        break;

    case pf_uart_timer_1:
        this->m_timer1.setEnable(val);
        break;

    default:
        break;
    }

    return 0;
}

/******************************************************************************/
/**
 * @brief   US_SetTransferNumber
 *
 *              
 *
 * @param[in]   val
 * @return      None
 */
/******************************************************************************/
int UartSim::US_SetReceiveEnable(unsigned int val)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_SetReceiveEnable(" << val << "): " << this->m_portname << std::endl;
#endif

    this->m_RE = val;
    if (this->m_RE == 1)
    {
        boost::lock_guard < boost::mutex > lock(this->thread_mutex);
        this->thread_state = HAS_WORK;
        this->thread_condition.notify_one();
    }
    return 0;
}

/******************************************************************************/
/**
 * @brief   US_SetTransferNumber
 *
 *              
 *
 * @param[in]   val
 * @return      None
 */
/******************************************************************************/
int UartSim::US_ClearReceiveFifo(unsigned int val)
{
#ifdef UARTSIM_DEBUG
    std::cout << "UartSim::US_SetTransmitEnable(" << val << "): " << this->m_portname << std::endl;
#endif

    memset(m_recv_buf, 0x00, sizeof(m_recv_buf));
    m_recv_len = 0;
    return 0;
}
