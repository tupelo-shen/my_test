#include "APP_UartCom.h"

/******************************************************************************/
/**
 * @brief   constructor
 *
 *              
 *
 * @param[in]   none
 * @return      none
 */
/******************************************************************************/
UartCom::UartCom()
{
    this->file_handler = INVALID_FILE_HANDLE;
    this->uart_err = 0;
    this->listener = NULL;
    this->baudrate = 0;
    this->recv_fifo_sz_timer0_tmout = 0;
}

/******************************************************************************/
/**
 * @brief   init
 *
 *              
 *
 * @param[in]   none
 * @return      none
 */
/******************************************************************************/
bool UartCom::init(ModbusCommon::UART_TYPE type, const pf_uart_config_t* config, 
        ComListener* listener)
{
    this->listener = listener;
    return reset(type, config);
}

/******************************************************************************/
/**
 * @brief   reset
 *
 *              
 *
 * @param[in]   none
 * @return      none
 */
/******************************************************************************/
bool UartCom::reset(ModbusCommon::UART_TYPE type, const pf_uart_config_t* config)
{
    bool ret = false;
    if (ModbusCommon::UART_MAX <= type)
    {
        return ret;
    }

    const char* portname = UART_PORTNAME[type];
    this->baudrate = _get_baudrate(config->baudrate);

    if (INVALID_FILEHANDLE != this->file_handler)
    {
        if (0 != _sys_close(this->file_handler))
        {
            return ret;   // _sys_close() 失敗
        }
    }

    if (0 == PF_UART_ioctl(portname, config, cb_event_func, this))
    {
        this->file_handler = _sys_open(portname, 0);
        if (PF_UART_OPEN_ERROR != this->file_handler)
        {
            /* enabled interrupt */
            PF_UART_SetReceiveFifoSize(this->file_handler, 3);
            PF_UART_SetTransmitEnable(this->file_handler, 0);   // TE = 0
            PF_UART_SetInterruptEnable(this->file_handler, UART_INTR_ENABLE, 1);

            ret = true;
        }
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief   reset
 *
 *              
 *
 * @param[in]   none
 * @return      none
 */
/******************************************************************************/
void UartCom::uart_event_proc(unsigned int event)
{
    if ((event & PF_UART_EVENT_TX_FRAME_COMPLETE) | (event & PF_UART_EVENT_TX_DATA_COMPLETE))
    {
        send_frame_completed();
    }
    if (event & PF_UART_EVENT_RX_TIMER0_TIMEOUT)
    {
        timer0_timeout();
    }
    if (event & PF_UART_EVENT_RX_TIMER1_TIMEOUT)
    {
        timer1_timeout();
    }
    if (event & PF_UART_EVENT_RX_FIFO_FULL)
    {
        recv_fifo_full();
    }

    PF_UART_SetStatus(this->file_handler, event, 1);
}

/******************************************************************************/
/**
 * @brief   getBaudrate
 *
 *              
 *
 * @param[in]   none
 * @return      none
 */
/******************************************************************************/
unsigned int UartCom::getBaudrate()
{
    return this->baudrate;
}

/******************************************************************************/
/**
 * @brief   _get_timeout
 *
 *              
 *
 * @param[in]   none
 * @return      none
 */
/******************************************************************************/
unsigned int UartCom::_get_timeout(TIMEOUT_TYPE type, int msec)
{
    float time;

    if (type == TOUT_RXGAP)
    {
        // Use as reception gap timer
        time = ((msec / 1000.f) / (1 / (float)this->baudrate) + 1.02f) * 16 + 1 + 8;
    } else /*if (type == TOUT_TFRMEND)*/
    {
        // Use as a simple timer that starts counting in response to a frame transmission completion interrupt
        time = ((msec / 1000.f) / (1 / (float)this->baudrate)) * 16 + 8;
    }

    return static_cast<unsigned int>(time);
}

/******************************************************************************/
/**
 * @brief   _get_timeout
 *
 *              
 *
 * @param[in]   type:       timer type
 * @param[in]   ch:         cost time for recieving a charactor
 * @return      time:       timeout
 */
/******************************************************************************/
unsigned int UartCom::_get_ch_time(TIMEOUT_TYPE type, float ch)
{
    float time;

    if (type == TOUT_RXGAP)
    {
        // Use as reception gap timer
        time = (float)(11 * ch + 1.02f) * 16 + 1 + 8;
    } else /*if (type == TOUT_TFRMEND)*/
    {
        // Use as a simple timer that starts counting in response to a frame transmission completion interrupt
        time = (float)(11 * ch) * 16 + 8;
    }

    return static_cast<unsigned int>(time);
}

/******************************************************************************/
/**
 * @brief   _get_baudrate
 *
 *              
 *
 * @param[in]   baudrate_t: configuring uart baudrate
 * @return      baudrate 
 */
/******************************************************************************/
unsigned int UartCom::_get_baudrate(pf_uart_baudrate_t baudrate_t)
{
    unsigned int baudrate = 0;
    switch (baudrate_t)
    {
    case pf_uart_baudrate_t::pf_baudrate_1200:
        baudrate = 1200;
        break;

    case pf_uart_baudrate_t::pf_baudrate_4800:
        baudrate = 4800;
        break;

    case pf_uart_baudrate_t::pf_baudrate_9600:
        baudrate = 9600;
        break;

    case pf_uart_baudrate_t::pf_baudrate_19200:
        baudrate = 19200;
        break;

    case pf_uart_baudrate_t::pf_baudrate_38400:
        baudrate = 38400;
        break;

    case pf_uart_baudrate_t::pf_baudrate_115200:
        baudrate = 115200;
        break;

    case pf_uart_baudrate_t::pf_baudrate_reserved:
    default:
        baudrate = 9600;
        break;
    }

    return baudrate;
}
