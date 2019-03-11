#include "PF_UART_Driver.h"
#include "SIM_UartSim.h"
#include "SIM_UartSimHart.h"

#define INVALID_FILE_HANDLE         (-1)
#define MAX_SERIAL_PORT             (5)         // Sensor module 1/2, hart, rs485, bluetooth

// static global variable for this file
static UartSim              serial_mod[MAX_SERIAL_PORT];
static UartSimHart          serial_hart;
static int                  hart_port = 0;
static const char           HART_PORT_NAME[] = ":UART2";
// UART ports list
FILE_HANDLE fh_list[MAX_SERIAL_PORT] = 
{
    INVALID_FILE_HANDLE, 
    INVALID_FILE_HANDLE, 
    INVALID_FILE_HANDLE, 
    INVALID_FILE_HANDLE,
    INVALID_FILE_HANDLE
};

/******************************************************************************/
/**
 * @brief   getSerial
 *
 *              
 *
 * @param[in]   fh      serial port file handler
 * @return      s:      serial port
 */
/******************************************************************************/
UartSim* getSerial(FILE_HANDLE fh)
{
    UartSim* s = NULL;
    if (fh != INVALID_FILE_HANDLE)
    {
        for (int i = 0; i < MAX_SERIAL_PORT; ++i)
        {
            if (fh_list[i] == fh)
            {
                if (fh == hart_port)
                {
                    s = &serial_hart;
                }
                else
                {
                    s = &serial_mod[i];
                }
                break;
            }
        }
    }

    return s;
}

/******************************************************************************/
/**
 * @brief   _sys_open
 *
 *              
 *
 * @param[in]   name        serial port name
 * @return      open_mode 
 */
/******************************************************************************/
FILE_HANDLE _sys_open(const char * name, int open_mode)
{
    FILE_HANDLE fh = INVALID_FILE_HANDLE;    // return value

    for (int i = 0; i < MAX_SERIAL_PORT; i++)
    {
        if (fh_list[i] == INVALID_FILE_HANDLE)
        {
            if (!strcmp(HART_PORT_NAME, name))
            {
                serial_hart.US_Open(name, open_mode);
                if (serial_hart.getPortName() == 0x0)
                {
                    // failed to open the serial port
                    return INVALID_FILE_HANDLE;
                }
                hart_port = i+1;
            }
            else
            {
                // if not open, open com port
                serial_mod[i].US_Open(name, open_mode);
                if (serial_mod[i].getPortName() == 0x0)
                {
                    // failed to open com port
                    return INVALID_FILE_HANDLE;
                }
            }
            fh_list[i] = i+1;
            fh = fh_list[i];
            break;
        }
    }

    return fh;
}

/******************************************************************************/
/**
 * @brief   _sys_close
 *
 *              
 *
 * @param[in]   fh
 * @return      0
 */
/******************************************************************************/
int _sys_close(FILE_HANDLE fh)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        s->US_close();
    }
    return 0;
}

/******************************************************************************/
/**
 * @brief   _sys_close
 *
 *              
 *
 * @param[in]   fh
 * @return      0
 */
/******************************************************************************/
int _sys_write(FILE_HANDLE fh, const unsigned char * buf, unsigned int len, int mode)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_write(fh, buf, len, mode);
    }else
    {
        return 0;
    }
}

/******************************************************************************/
/**
 * @brief   _sys_close
 *
 *              
 *
 * @param[in]   fh
 * @return      0
 */
/******************************************************************************/
int _sys_read(FILE_HANDLE fh, unsigned char * buf, unsigned int len, int mode)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_read(fh, buf, len, mode);
    }else
    {
        return 0;
    }
}

/******************************************************************************/
/**
 * @brief   _sys_close
 *
 *              
 *
 * @param[in]   fh
 * @return      0
 */
/******************************************************************************/
int PF_UART_ioctl(const char *name, const pf_uart_config_t *config, 
                PF_UART_Event_t cb_event, void* userData)
{
    for(int i = 0;i < MAX_SERIAL_PORT;i++)
    {
        if (i+1 == hart_port)
        {
            if (serial_hart.getPortName() != 0x0 && strcmp(serial_hart.getPortName(), name) == 0)
            {
                // if not open, open com port
                return serial_hart.US_ioctl(name, config, cb_event, userData);
            }
        }
        else
        {
            if (serial_mod[i].getPortName() != 0x0 && strcmp(serial_mod[i].getPortName(), name) == 0)
            {
                // if not open, open com port
                return serial_mod[i].US_ioctl(name, config, cb_event, userData);
            }
        }
    }

    return 0;
}
/* the functions be used as setting the serial ports */
int PF_UART_SetTransmitEnable(FILE_HANDLE fh, unsigned int val)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_SetTransmitEnable(val);
    }else
    {
        return 0;
    }
}

int PF_UART_SetReceiveEnable(FILE_HANDLE fh, unsigned int val)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_SetReceiveEnable(val);
    }else
    {
        return 0;
    }
}

int PF_UART_SetInterruptEnable(FILE_HANDLE fh, unsigned int interrupt, unsigned int val)
{
    return 0;
}

unsigned int PF_UART_GetStatus(FILE_HANDLE fh)
{
    return 0;
}

int PF_UART_SetStatus(FILE_HANDLE fh, unsigned int bit, unsigned int val)
{
    return 0;
}

int PF_UART_SetReceiveFifoSize(FILE_HANDLE fh, unsigned int val)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_SetReceiveFifoSize(val);
    }else
    {
        return 0;
    }
}

int PF_UART_GetReceiveFifoTriggerSize(FILE_HANDLE fh)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_GetReceiveFifoTriggerSize();
    }else
    {
        return 0;
    }
}

int PF_UART_SetTransmitFifoSize(FILE_HANDLE fh, unsigned int val)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_SetTransmitFifoSize(val);
    }else
    {
        return 0;
    }
}

int PF_UART_ClearTransmitFifo(FILE_HANDLE fh, unsigned int val)
{
    return 0;
}

int PF_UART_ClearReceiveFifo(FILE_HANDLE fh, unsigned int val)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_ClearReceiveFifo(val);
    }else
    {
        return 0;
    }
}

int PF_UART_SetLoopbackTest(FILE_HANDLE fh, unsigned int val)
{
    return 0;
}

int PF_UART_GetTransmitFifoSize(FILE_HANDLE fh)
{
    return 0;
}

int PF_UART_GetReceiveFifoSize(FILE_HANDLE fh)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->m_recv_len;
    }else
    {
        return 0;
    }
}

int PF_UART_TimerStart(FILE_HANDLE fh, pf_uart_timer_t timer, unsigned int val)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_TimerStart(timer, val);
    }else
    {
        return 0;
    }
}

int PF_UART_TransmitCompleteTimerStart(FILE_HANDLE fh, pf_uart_timer_t timer, unsigned int val)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_TransmitCompleteTimerStart(timer, val);
    }else
    {
        return 0;
    }
}

int PF_UART_ReceiveGapTimerStart(FILE_HANDLE fh, pf_uart_timer_t timer, unsigned int val)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_ReceiveGapTimerStart(timer, val);
    }else
    {
        return 0;
    }
}

int PF_UART_SetReceiveDataReadyEnable(FILE_HANDLE fh, unsigned int val)
{
    return 0;
}

int PF_UART_SetTimerEnable(FILE_HANDLE fh, pf_uart_timer_t timer, unsigned int val)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_SetTimerEnable(timer, val);
    }else
    {
        return 0;
    }
}

int PF_UART_SetAutoTransferEnable(FILE_HANDLE fh, unsigned int val)
{
    return 0;
}

int PF_UART_SetTimer(FILE_HANDLE fh, pf_uart_timer_t timer, unsigned int val)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_SetTimer(timer, val);
    }else
    {
        return 0;
    }
}

int PF_UART_SetTransferNumber(FILE_HANDLE fh, unsigned int val)
{
    UartSim* s = getSerial(fh);
    if(s)
    {
        return s->US_SetTransferNumber(val);
    }else
    {
        return 0;
    }
}

void PF_UART_GetFpgaVersion(FILE_HANDLE fh, PF_DRIVER_VERSION *version)
{
    version->ver = 1; /**> Version */
    version->rev = 2; /**> Revision */
}

void PF_UART_GetHartFpgaRevision(PF_DRIVER_VERSION *version)
{
    version->ver = 3; /**> Version */
    version->rev = 4; /**> Revision */
}

int PF_UART_SetHartInterruptEnable(FILE_HANDLE fh, unsigned int interrupt, unsigned int val)
{
    return 0;
}

int PF_UART_SetHARTStatus(FILE_HANDLE fh, unsigned int bit, unsigned int val)
{
    return 0;
}

unsigned int PF_UART_GetHARTStatus(FILE_HANDLE fh)
{
    /* シミュレータではキャリア検出しない */
    return 0;
}