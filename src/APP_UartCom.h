/*
 * APP_UartCom.h
 */
#ifndef __APP_UARTCOM_H__
#define __APP_UARTCOM_H__

class UartCom
{
protected:
    static const char*          UART_PORTNAME[];
    static const unsigned int   UART_INTR_ENABLE;
    static const unsigned short SIZE_ADU_MAX = 256;

    /*
     * @enum->Timeout Type
     */
    enum TIMEOUT_TYPE
    {
        TOUT_RXGAP,
        TOUT_TFRMEND
    };

    unsigned int                baudrate;
    ComListener*                listener;                       /*!< Modbus communication listener*/
    unsigned short              uart_err;                       /*!< UART error */
    int                         recv_fifo_sz_timer0_tmout;      /*!< 受信FIFO数（タイマー0タイムアウト） */

    FILEHANDLE                  file_handler;

public:
    static const long           RECV_RETRY_COUNT;               /*!< 受信リトライ数 */
    static const int            INVALID_FILEHANDLE;             /*!< 無効ファイルハンドル */

public:
    UartCom();
    virtual ~UartCom() {};

    bool                    init(ModbusCommon::UART_TYPE type, 
                                const pf_uart_config_t* config, 
                                ComListener* listener);
    bool                    reset(ModbusCommon::UART_TYPE type, 
                            const pf_uart_config_t* config);
    void                    uart_event_proc(unsigned int event);

    virtual void            recv_fifo_full(void) = 0;
    virtual void            timer0_timeout(void) = 0;
    virtual void            timer1_timeout(void) = 0;
    virtual void            send_frame_completed(void) = 0;
    virtual void            errors(unsigned short err) {};

    unsigned int            getBaudrate();  

protected:
    unsigned int            _get_timeout(TIMEOUT_TYPE type, int msec);
    unsigned int            _get_ch_time(TIMEOUT_TYPE type, float ch);
    unsigned int            _get_baudrate(pf_uart_baudrate_t baudrate_t); 
};
#endif /* __APP_UARTCOM_H__ */
