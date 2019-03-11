#ifndef __PF_UART_DRIVER_H_
#define __PF_UART_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#define  FILE_HANDLE         int

/*!
\brief エラー情報を示す列挙型
*/
enum
{
  PF_UART_OK = 0,                   /**< 正常終了 */
  PF_UART_OPEN_ERROR = -1,          /**< オープン失敗 */
  PF_UART_PARAMETER_ERROR = -2,     /**< パラメータエラー */
};

/******************************************************************************/
/*!
\brief       UARTをオープンする
\param[in]   name オープンするUART番号 文字列へのポインタ \n
             nameに渡す引数は :UART0, :UART1, :UART2, :UART3, :UART4
\param[in]   openmode UARTでは無視される
\return      正常終了: オープンした際のファイルハンドルを返す \n
             エラー発生: オープンできない場合はPF_UART_OPEN_ERRORを返す
*******************************************************************************/
extern FILE_HANDLE _sys_open(const char * name, int openmode);

/******************************************************************************/
/*!
\brief       UARTをクローズする
\param[in]   fh ファイルハンドラ
\return      正常終了: 0 が返されます。 \n
エラー発生: ゼロ以外の値が返されます。
*******************************************************************************/
extern int _sys_close(FILE_HANDLE fh);

/******************************************************************************/
/*!
\brief       UARTへの書き込み
\param[in]   fh ファイルハンドラ
\param[in]   buf 書き込むための文字へのポインタ
\param[in]   len 書き込む長さ
\param[in]   mode ログに記録される任意の値 (呼び出し側のTaskID等を入力する)
\return      正常終了: 0
             エラー発生 ・正数: 書き込まれなかった文字の数を表す \n
                        ・負数: エラーを示す負の値
*******************************************************************************/
extern int _sys_write(FILE_HANDLE fh, const unsigned char * buf,
                      unsigned int len, int mode);

/******************************************************************************/
/*!
\brief       UARTからの読み出し
\param[in]   fh ファイルハンドラ
\param[in]   buf 読み込むための文字へのポインタ
\param[in]   len 読み込む長さ
\param[in]   mode ログに記録される任意の値 (呼び出し側のTaskID等を入力する)
\return      正常終了: 0 \n
             エラー発生 ・正数: 読み出されなかった文字の数 (つまり、読み出された数は len - result）\n
                        ・負数: エラーを示す負の値
*******************************************************************************/
extern int _sys_read(FILE_HANDLE fh, unsigned char * buf,
                     unsigned int len, int mode);

/*!
\brief イベント通知のコールバック関数ポインタ
\param[in] event 発生イベント
\param[in] user_data ユーザーデータ
*/
typedef void (*PF_UART_Event_t)(unsigned int event, void *user_data);

/*!
\brief Baud Rate 列挙型
*/
typedef enum
{
  pf_baudrate_1200 = 0, /**< 1,200 baud */
  pf_baudrate_4800,     /**< 4,800 baud */
  pf_baudrate_9600,     /**< 9,600 baud */
  pf_baudrate_19200,    /**< 19,200 baud */
  pf_baudrate_38400,    /**< 38,400 baud */
  pf_baudrate_115200,    /**< 115,200 baud */

  pf_baudrate_reserved = 0x7FFFFFFF /**< prevent from enum down-size compiler optimization */
} pf_uart_baudrate_t;

/*!
\brief Parity 列挙型
*/
typedef enum
{
  pf_parity_none = 0, /**< パリティ無し */
  pf_parity_odd = 1,  /**< 奇数パリティ */
  pf_parity_even = 2,  /**< 偶数パリティ */

  pf_parity_reserved = 0x7FFFFFFF /**< prevent from enum down-size compiler optimization */
} pf_uart_parity_t;

/*!
\brief データ長 列挙型
*/
typedef enum
{
  pf_databit_7 = 7, /**< 7 bit */
  pf_databit_8 = 8,  /**< 8 bit */

  pf_databit_reserved = 0x7FFFFFFF /**< prevent from enum down-size compiler optimization */
} pf_uart_databit_t;

/*!
\brief ストップビット 列挙型
*/
typedef enum
{
  pf_stopbit_1 = 1, /**< 1 bit */
  pf_stopbit_2 = 2,  /**< 2 bit */

  pf_stopbit_reserved = 0x7FFFFFFF /**< prevent from enum down-size compiler optimization */
} pf_uart_stopbit_t;

/*!
\brief フロー制御 列挙型
*/
typedef enum
{
  pf_flowcontrol_none = 0,  /**< フロー制御無し */
  pf_flowcontrol_rts = 1,    /**< RTS-CTS によるハードウェアフロー制御有り */

  pf_flowcontrol_reserved = 0x7FFFFFFF /**< prevent from enum down-size compiler optimization */
} pf_uart_flowcontrol_t;

/*!
\brief RTSアクティブになる数 列挙型
*/
typedef enum
{
  pf_rts_active_511 = 0,  /**< 511 */
  pf_rts_active_1,        /**< 1 */
  pf_rts_active_64,       /**< 64 */
  pf_rts_active_128,      /**< 128 */
  pf_rts_active_256,      /**< 256 */
  pf_rts_active_384,      /**< 384 */
  pf_rts_active_432,      /**< 432 */
  pf_rts_active_480,       /**< 480 */

  pf_rts_active_reserved = 0x7FFFFFFF /**< prevent from enum down-size compiler optimization */
} pf_uart_rts_active_t;

/*!
\brief 極性論理 列挙型
*/
typedef enum
{
  pf_polarity_0 = 0,  /**< 極性論理反転なし */
  pf_polarity_1 = 1,   /**< 極性論理反転あり */

  pf_polarity_reserved = 0x7FFFFFFF /**< prevent from enum down-size compiler optimization */
} pf_uart_polarity_t;

/*!
\brief UART configuration structure
*/
typedef struct
{
  pf_uart_baudrate_t    baudrate;           /**< Baud Rate */
  pf_uart_parity_t      parity;             /**< Parity */
  pf_uart_databit_t     databit;            /**< data bits */
  pf_uart_stopbit_t     stopbit;            /**< stop bit */
  pf_uart_flowcontrol_t flowcontrol;        /**< flow control */
  pf_uart_rts_active_t  rts;                /**< rts */
  pf_uart_polarity_t    polarity;           /**< polarity */
} pf_uart_config_t;

/*!
\brief タイマー種類 列挙型
*/
typedef enum
{
  pf_uart_timer_0 = 0, /**< タイマー0 */
  pf_uart_timer_1 = 1,  /**< タイマー1 */

  pf_uart_timer_reserved = 0x7FFFFFFF /**< prevent from enum down-size compiler optimization */
} pf_uart_timer_t;

/****** UART Event *****/
#define PF_UART_EVENT_TX_FRAME_COMPLETE   (1UL << 15) /**< フレーム送信完了 */
#define PF_UART_EVENT_TX_DATA_COMPLETE    (1UL << 14) /**< データ送信完了 */
#define PF_UART_EVENT_RX_OVERRUN          (1UL << 11) /**< オーバーランエラー */
#define PF_UART_EVENT_RX_TIMER1_TIMEOUT   (1UL << 9)  /**< タイマ1のタイムアウト */
#define PF_UART_EVENT_RX_TIMER0_TIMEOUT   (1UL << 8)  /**< タイマ0のタイムアウト */
#define PF_UART_EVENT_RX_RECEIVE_ERROR    (1UL << 7)  /**< レシーブエラー */
                                                      /* トランスミットエンド */  /* 割り込みなし */
#define PF_UART_EVENT_TX_FIFO_EMPTRY      (1UL << 5)  /**< トランスミットFIFOデータエンプティ */
#define PF_UART_EVENT_BREAK_DETECT        (1UL << 4)  /**< ブレーク検出 */
#define PF_UART_EVENT_RX_FRAMING_ERROR    (1UL << 3)  /**< フレーミングエラー */
#define PF_UART_EVENT_RX_PARITY_ERROR     (1UL << 2)  /**< パリティエラー */
#define PF_UART_EVENT_RX_FIFO_FULL        (1UL << 1)  /**< レシーブFIFOデータフル */
#define PF_UART_EVENT_RX_READY            (1UL << 0)  /**< レシーブデータレディ */
#define PF_UART_EVENT_RX_HART_PREAMBLE    (1UL << 17) /**< HARTプリアンブル受信 */
#define PF_UART_EVENT_RX_HART_FRAME_COMPLETE (1UL << 16)  /**< HARTフレーム受信完了 */
                                                      /* HARTキャリア検出 */ /* 割り込みなし */
#define PF_UART_EVENT_DMA_READ_COMPLETE   (1UL << 20) /**< DMA読み込み完了 */
#define PF_UART_EVENT_DMA_WRITE_COMPLETE  (1UL << 21) /**< DMA書き込み完了 */
#define PF_UART_EVENT_DMA_ERROR           (1UL << 22) /**< DMAエラー発生 */

/******************************************************************************/
/*!
\brief      TX Enable
\param[in]  fh ファイルハンドラ
\param[in]  val 設定値
\return     正常終了: 0  \n
            エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetTransmitEnable(FILE_HANDLE fh, unsigned int val);

/******************************************************************************/
/*!
\brief      Rx Enable
\param[in]  fh ファイルハンドラ
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetReceiveEnable(FILE_HANDLE fh, unsigned int val);

#define PF_UART_INT_TX_FRAME_COMPLETE_MASK  (1UL << 15) /**< フレーム送信完了 */
#define PF_UART_INT_TX_DATA_COMPLETE_MASK   (1UL << 14) /**< データ送信完了 */
#define PF_UART_INT_RX_OVERRUN_MASK         (1UL << 11) /**< オーバーランエラー */
#define PF_UART_INT_RX_TIMER1_TIMEOUT_MASK  (1UL << 9)  /**< タイマ1のタイムアウト */
#define PF_UART_INT_RX_TIMER0_TIMEOUT_MASK  (1UL << 8)  /**< タイマ0のタイムアウト */
#define PF_UART_INT_RX_RECEIVE_ERROR_MASK   (1UL << 7)  /**< レシーブエラー */
                                                        /* トランスミットエンド */  /* 割り込みなし */
#define PF_UART_INT_TX_FIFO_EMPTRY_MASK     (1UL << 5)  /**< トランスミットFIFOデータエンプティ */
#define PF_UART_INT_BREAK_DETECT_MASK       (1UL << 4)  /**< ブレーク検出 */
#define PF_UART_INT_RX_FRAMING_ERROR_MASK   (1UL << 3)  /**< フレーミングエラー */
#define PF_UART_INT_RX_PARITY_ERROR_MASK    (1UL << 2)  /**< パリティエラー */
#define PF_UART_INT_RX_FIFO_FULL_MASK       (1UL << 1)  /**< レシーブFIFOデータフル */
#define PF_UART_INT_RX_READY_MASK           (1UL << 0)  /**< レシーブデータレディ */
/******************************************************************************/
/*!
\brief      割り込み設定。立っているビットに該当する割り込み設定を行う。
複数ビット対応。予約場所にビットが立っている場合は書き込まない。
\param[in]  fh ファイルハンドラ
\param[in]  interrupt 設定する割り込み
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetInterruptEnable(FILE_HANDLE fh, unsigned int interrupt, unsigned int val);

#define PF_UART_STAT_TX_FRAME_COMPLETE_MASK   (1UL << 15) /**< フレーム送信完了 */
#define PF_UART_STAT_TX_DATA_COMPLETE_MASK    (1UL << 14) /**< データ送信完了 */
#define PF_UART_STAT_RX_OVERRUN_MASK          (1UL << 11) /**< オーバーランエラー */
#define PF_UART_STAT_RX_TIMER1_TIMEOUT_MASK   (1UL << 9)  /**< タイマ1のタイムアウト */
#define PF_UART_STAT_RX_TIMER0_TIMEOUT_MASK   (1UL << 8)  /**< タイマ0のタイムアウト */
#define PF_UART_STAT_RX_RECEIVE_ERROR_MASK    (1UL << 7)  /**< レシーブエラー */
#define PF_UART_STAT_TX_END_MASK              (1UL << 6)  /**< トランスミットエンド */
#define PF_UART_STAT_TX_FIFO_EMPTRY_MASK      (1UL << 5)  /**< トランスミットFIFOデータエンプティ */
#define PF_UART_STAT_BREAK_DETECT_MASK        (1UL << 4)  /**< ブレーク検出 */
#define PF_UART_STAT_RX_FRAMING_ERROR_MASK    (1UL << 3)  /**< フレーミングエラー */
#define PF_UART_STAT_RX_PARITY_ERROR_MASK     (1UL << 2)  /**< パリティエラー */
#define PF_UART_STAT_RX_FIFO_FULL_MASK        (1UL << 1)  /**< レシーブFIFOデータフル */
#define PF_UART_STAT_RX_READY_MASK            (1UL << 0)  /**< レシーブデータレディ */
/******************************************************************************/
/*!
\brief      ステータス取得
\param[in]  fh ファイルハンドラ
\return     ステータス値
*******************************************************************************/
unsigned int PF_UART_GetStatus(FILE_HANDLE fh);

/******************************************************************************/
/*!
\brief      ステータス設定
\param[in]  fh ファイルハンドラ
\param[in]  bit 設定するビット
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetStatus(FILE_HANDLE fh, unsigned int bit, unsigned int val);

/******************************************************************************/
/*!
\brief      レシーブFIFOトリガデータ数設定関数
\param[in]  fh ファイルハンドラ
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetReceiveFifoSize(FILE_HANDLE fh, unsigned int val);

/******************************************************************************/
/*!
\brief      レシーブFIFOトリガデータ数取得関数
\param[in]  fh ファイルハンドラ
\return     正常終了:レシーブFIFOトリガデータ数, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_GetReceiveFifoTriggerSize(FILE_HANDLE fh);

/******************************************************************************/
/*!
\brief      トランスミットFIFOトリガデータ数設定関数
\param[in]  fh ファイルハンドラ
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetTransmitFifoSize(FILE_HANDLE fh, unsigned int val);

/******************************************************************************/
/*!
\brief      トランスミットFIFOトリガデータ数取得関数
\param[in]  fh ファイルハンドラ
\return     正常終了:トランスミットFIFOトリガデータ, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_GetTransmitFifoTriggerSize(FILE_HANDLE fh);

/******************************************************************************/
/*!
\brief      トランスミットFIFOデータ数レジスタクリア関数
\param[in]  fh ファイルハンドラ
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_ClearTransmitFifo(FILE_HANDLE fh, unsigned int val);

/******************************************************************************/
/*!
\brief      レシーブFIFOデータ数レジスタクリア
\param[in]  fh ファイルハンドラ
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_ClearReceiveFifo(FILE_HANDLE fh, unsigned int val);

/******************************************************************************/
/*!
\brief      ループバックテスト開始関数
\param[in]  fh ファイルハンドラ
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetLoopbackTest(FILE_HANDLE fh, unsigned int val);

/******************************************************************************/
/*!
\brief      トランスミットFIFOデータ数取得関数
\param[in]  fh ファイルハンドラ
\return     Tx FIFOデータ数
*******************************************************************************/
int PF_UART_GetTransmitFifoSize(FILE_HANDLE fh);

/******************************************************************************/
/*!
\brief      レシーブFIFOデータ数取得関数
\param[in]  fh ファイルハンドラ
\return     Rx FIFOデータ数
*******************************************************************************/
int PF_UART_GetReceiveFifoSize(FILE_HANDLE fh);

/******************************************************************************/
/*!
\brief      タイマのカウンタ開始トリガ
\param[in]  fh ファイルハンドラ
\param[in]  timer タイマ種別
\param[in]  val 設定値
\return     正常終了: 0 \n
エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_TimerStart(FILE_HANDLE fh, pf_uart_timer_t timer, unsigned int val);

/******************************************************************************/
/*!
\brief      送信完了のカウンタ開始トリガ
\param[in]  fh ファイルハンドラ
\param[in]  timer タイマ種別
\param[in]  val 設定値
\return     正常終了: 0 \n
エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_TransmitCompleteTimerStart(FILE_HANDLE fh, pf_uart_timer_t timer, unsigned int val);

/******************************************************************************/
/*!
\brief      受信ギャップタイマのカウンタ開始トリガ
\param[in]  fh ファイルハンドラ
\param[in]  timer タイマ種別
\param[in]  val 設定値
\return     正常終了: 0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_ReceiveGapTimerStart(FILE_HANDLE fh, pf_uart_timer_t timer, unsigned int val);

/******************************************************************************/
/*!
\brief      レシーブデータレディDRのイネーブル
\param[in]  fh ファイルハンドラ
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetReceiveDataReadyEnable(FILE_HANDLE fh, unsigned int val);

/******************************************************************************/
/*!
\brief      タイマのイネーブル
\param[in]  fh ファイルハンドラ
\param[in]  timer タイマ種別
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetTimerEnable(FILE_HANDLE fh, pf_uart_timer_t timer, unsigned int val);

/******************************************************************************/
/*!
\brief      Auto Transfer設定(レジスタTEの自動セット・クリア、及びレジスタREの自動クリア)
\param[in]  fh ファイルハンドラ
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetAutoTransferEnable(FILE_HANDLE fh, unsigned int val);

/******************************************************************************/
/*!
\brief      タイマのタイムアウト時間設定
\param[in]  fh ファイルハンドラ
\param[in]  timer タイマ種別
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetTimer(FILE_HANDLE fh, pf_uart_timer_t timer, unsigned int val);

/******************************************************************************/
/*!
\brief      送信データ数設定 (1オリジン、受け取った値をドライバが-1して設定する。)
\param[in]  fh ファイルハンドラ
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetTransferNumber(FILE_HANDLE fh, unsigned int val);

/*!
\brief バージョンを示す構造体
*/
typedef struct {
  unsigned short ver; /**< Version */
  unsigned short rev; /**< Revision */
} PF_DRIVER_VERSION;

/******************************************************************************/
/*!
\brief      FPGAバージョン情報取得
\param[in]  fh ファイルハンドラ
\param[out] version バージョン構造体へのポインタ
\return     無し
*******************************************************************************/
void PF_UART_GetFpgaVersion(FILE_HANDLE fh, PF_DRIVER_VERSION *version);

/******************************************************************************/
/*!
\brief      HART回路バージョン取得
\param[in]  fh ファイルハンドラ
\param[out] version バージョン構造体へのポインタ
\return     無し
*******************************************************************************/
void PF_UART_GetHartFpgaVersion(FILE_HANDLE fh, PF_DRIVER_VERSION *version);

#define PF_UART_INT_RX_HART_PREAMBLE            (1UL << 2)  /**< HARTプリアンブル受信 */
#define PF_UART_INT_RX_HART_FRAME_COMPLETE_MASK (1UL << 1)  /**< HARTフレーム受信完了 */
/******************************************************************************/
/*!
\brief      HART割り込み設定
\param[in]  fh ファイルハンドラ
\param[in]  interrupt 設定する割り込み
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetHartInterruptEnable(FILE_HANDLE fh, unsigned int interrupt, unsigned int val);

#define PF_UART_STAT_RX_HART_PREAMBLE             (1UL << 2)  /**< HARTプリアンブル受信 */
#define PF_UART_STAT_RX_HART_FRAME_COMPLETE_MASK  (1UL << 1)  /**< HARTフレーム受信完了 */
#define PF_UART_STAT_RX_CARRIER_DETECT            (1UL << 0)  /**< HARTキャリア検出 */
/******************************************************************************/
/*!
\brief      HARTステータス取得
\param[in]  fh ファイルハンドラ
\return     ステータス値
*******************************************************************************/
unsigned int PF_UART_GetHARTStatus(FILE_HANDLE fh);

/******************************************************************************/
/*!
\brief      HARTステータス設定
\param[in]  fh ファイルハンドラ
\param[in]  bit 設定するビット
\param[in]  val 設定値
\return     正常終了:0, エラー発生 PF_UART_PARAMETER_ERROR: パラメータ不正
*******************************************************************************/
int PF_UART_SetHARTStatus(FILE_HANDLE fh, unsigned int bit, unsigned int val);

#ifdef __cplusplus
}
#endif

#endif /* __PF_UART_DRIVER_H_ */
