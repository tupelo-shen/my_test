//#include <iostream>

// #include "liq-4wire/Modbus/UartCom.h"

#include "SIM_UartSimHart.h"

enum STATE_FPGA_HART
{
    PREAMBLE_1ST = 0, //1つ目のpreamble待ち
    PREAMBLE_2ND,     //2つ目のpreamble待ち
    DELIMITER,        //デリミタ待ち
    CMDBYTE,          //コマンドバイト待ち
    RX_COMPLETE,      //受信完了待ち
};

UartSimHart::UartSimHart()
{
}

UartSimHart::~UartSimHart()
{
}

unsigned long UartSimHart::recvPacket(unsigned char* pBuf, unsigned long buf_size)
{
    // データの受信
    unsigned long recv_len = 0;// return value:receive byte size
    // bool rx_complete = false;

    // while (recv_len == 0 && this->thread_state == NO_WORK)
    // {
    //     unsigned char c;
    //     STATE_FPGA_HART hart_state = PREAMBLE_1ST;
    //     unsigned char addr_offset = 0;
    //     unsigned char exp_byte    = 0;
    //     unsigned char cmd_len     = 0;

    //     if (this->m_pReader == 0x0)
    //     {
    //         return 0;
    //     }

    //     // 1バイトづつ受信し、フレーム受信完了もしくはバッファサイズオーバーで受信処理を終了
    //     while (this->m_pReader->read_char(c) && recv_len < buf_size && rx_complete == false)
    //     {
    //         this->m_timer0.cancelTimer(); /*タイマを停止*/

    //         switch(hart_state)
    //         {
    //         case PREAMBLE_1ST:
    //             if (0xFF == c)
    //             {
    //                 hart_state = PREAMBLE_2ND;
    //                 if (this->m_evt_func != NULL)
    //                 {
    //                     this->m_evt_func(PF_UART_EVENT_RX_HART_PREAMBLE, this->userData);
    //                 }
    //             }
    //             break;
    //         case PREAMBLE_2ND:
    //             if (0xFF == c)
    //             {
    //                 hart_state = DELIMITER;
    //             }
    //             else
    //             {
    //                 hart_state = PREAMBLE_1ST;
    //             }
    //             break;
    //         case DELIMITER:
    //             if (0xFF != c)
    //             {
    //                 hart_state = CMDBYTE;

    //                 /* 受信FIFOクリア(#15765) */
    //                 memset(m_recv_buf, 0x00, sizeof(m_recv_buf));
    //                 m_recv_len = 0;

    //                 pBuf[recv_len] = c; //デリミタ
    //                 recv_len++;
    //                 this->m_recv_len++;

    //                 /*アドレスオフセット計算*/
    //                 if (c & 0x80)
    //                     addr_offset = 5;
    //                 else
    //                     addr_offset = 1;

    //                 /*拡張バイト計算*/
    //                 exp_byte = (c & 0x60) >> 5;
    //             }
    //             break;
    //         case CMDBYTE:
    //             pBuf[recv_len] = c;
    //             recv_len++;
    //             this->m_recv_len++;
    //             if (recv_len > (addr_offset + exp_byte + 2U)) //2=Deli+Cmd
    //             {
    //                 cmd_len = c;
    //                 hart_state = RX_COMPLETE;
    //             }
    //             break;
    //         case RX_COMPLETE:
    //             pBuf[recv_len] = c;
    //             recv_len++;
    //             this->m_recv_len++;

    //             if (recv_len > ((addr_offset + exp_byte + 3U + cmd_len))) //3=Deli+Cmd+bytecnt
    //             {
    //                 rx_complete = true;
    //             }
    //         }

    //         if (this->m_recv_len == getFIFOTrg())
    //         {
    //             // call interrupt routine
    //             if (this->m_evt_func != NULL)
    //             {
    //                 this->m_evt_func(PF_UART_EVENT_RX_FIFO_FULL, this->userData);
    //             }
    //         }
    //     }
    // }

    // if (this->m_RE != 0 && this->m_evt_func != NULL && rx_complete == true)
    // {
    //     // フレーム受信完了
    //     this->m_evt_func(PF_UART_EVENT_RX_HART_FRAME_COMPLETE, this->userData);
    // }

    return recv_len;
}

// receive thread
void UartSimHart::run()
{
    // for (;;)
    // {
    //     {
    //         boost::unique_lock < boost::mutex > lock(this->thread_mutex);

    //         while (NO_WORK == this->thread_state)
    //         {
    //             this->thread_condition.wait(lock);
    //         }

    //         if (SHUTDOWN == this->thread_state)
    //             break;

    //         this->thread_state = NO_WORK;
    //     }

    //     {
    //         recvPacket(this->m_recv_buf, sizeof(this->m_recv_buf));
    //     }

    // }
}
/*
 *タイムアウト値をセットする。
 * HARTでFPGAに設定する値をms単位に置き換えている
 *
 * FPGA設定値はタイマのモードによって異なるが、
 * シミュ―レータであるため厳密な計算はしないものとする。
 */
int UartSimHart::US_SetTimer(pf_uart_timer_t timer, unsigned int val)
{
    // if (val < 30)         // 1 bit = 25 (STARTで計算)
    // {
    //     val = 1;          // 1 bit = 0.8ms
    // }
    // else if (val < 210)   // 1 char = 200.32 (RX_GAPで計算)
    // {
    //     //val = 9;          // 1 char time = 9.2ms
    //     // TODO: HART受信時にGAPタイムアウトが発生するようになった。
    //     //       暫定処理として設定時間を延ばす。
    //     val = 50;
    // }
    // else                  // 5 char time = 889 (STARTで計算)
    // {
    //     val = 45;         // 5 char time = 45.8ms
    // }

    // switch (timer)
    // {
    // case pf_uart_timer_0:
    //     this->m_timer0.setTimeout(val);
    //     break;

    // case pf_uart_timer_1:
    //     this->m_timer1.setTimeout(val);
    //     break;

    // default:
    //     break;
    // }
    return 0;

}

