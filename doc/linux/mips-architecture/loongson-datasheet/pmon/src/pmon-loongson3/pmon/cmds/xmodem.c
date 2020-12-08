#include <pmon.h>
#include <stdio.h>
#include <include/stdarg.h>
#include "ramfiles.h"
#undef XMODEM_DEBUG
static uint16_t _crc_xmodem_update (uint16_t crc, uint8_t data)
{
    int i;
    crc = crc ^ ((uint16_t)data << 8);
    for (i=0; i<8; i++)
    {
        if (crc & 0x8000)
            crc = (crc << 1) ^ 0x1021;
        else
            crc <<= 1;
    }
    return crc;
}



//�ܽŶ���
#define PIN_RXD            0   //PD0
#define PIN_TXD            1   //PD1

//��������
#define BLOCKSIZE       128            //M16��һ��FlashҳΪ128�ֽ�(64��)
#define DATA_BUFFER_SIZE    BLOCKSIZE   //������ջ���������
//#define F_CPU            7372800         //ϵͳʱ��7.3728MHz

//����Xmoden�����ַ�
#define XMODEM_NUL          0x00
#define XMODEM_SOH          0x01
#define XMODEM_STX          0x02
#define XMODEM_EOT          0x04
#define XMODEM_ACK          0x06
#define XMODEM_NAK          0x15
#define XMODEM_CAN          0x18
#define XMODEM_EOF          0x1A
#define XMODEM_WAIT_CHAR    'C'

//����ȫ�ֱ���
struct str_XMODEM
{
    unsigned char SOH;                  //��ʼ�ֽ�
    unsigned char BlockNo;               //���ݿ���
    unsigned char nBlockNo;               //���ݿ��ŷ���
    unsigned char Xdata[BLOCKSIZE];            //����128�ֽ�
    unsigned char CRC16hi;               //CRC16У�����ݸ�λ
    unsigned char CRC16lo;               //CRC16У�����ݵ�λ
}
strXMODEM;                           //XMODEM�Ľ������ݽṹ

static unsigned long FlashAddress;               //FLASH��ַ
#define  BootAdd          0x3800         //Boot�����׵�ַ(Ӧ��������ߵ�ַ)
/*   GCC�����ַʹ��32λ���ȣ���Ӧ����AVR������*/


static unsigned char STATUS;                  //����״̬
#define ST_WAIT_START       0x00         //�ȴ�����
#define ST_BLOCK_OK       0x01         //����һ�����ݿ�ɹ�
#define ST_BLOCK_FAIL       0x02         //����һ�����ݿ�ʧ��
#define ST_OK             0x03         //���


#ifdef XMODEM_DEBUG
#define MYDBG dbg_printf("%d\n",__LINE__);
static char dbgbuf[2048];
static char *pmsg=dbgbuf;
static int dbg=0;
static int dbg_printf (const char *fmt, ...)
{
    int  len;
    va_list	    ap;

    if(!dbg)return 0;

    va_start(ap, fmt);
    len= vsprintf (pmsg, fmt, ap);
	pmsg+=len;
    if((pmsg-dbgbuf)>1800)pmsg=dbgbuf;
    va_end(ap);
    return (len);
}

static int dmsg(int argc,char *argv[])
{
if(pmsg!=dbgbuf)
printf("%s\n",dbgbuf);
return 0;
}
#else
#define MYDBG 
#define dbg_printf(...)
#endif

static int testchar()
{
	int count=2;
	int total, start;
	start = CPU_GetCOUNT();

	while(1)
	{
    if(tgt_testchar())
	return 100;
	if(!count)break;
	if((CPU_GetCOUNT()-start>0x3000000 )){
	start = CPU_GetCOUNT();
	count--;
	}
	}

	   return 0;
}

static int get_data(unsigned char *ptr,unsigned int len,unsigned int timeout)
{
	int i=0;
	volatile int count=1;
	while(i<len)
	{
		if(testchar()>0)
		ptr[i++]=tgt_getchar();
		else {
		if(!count)break;
		dbg_printf("count=%d\n",count);
		count--;
		}
	}
	dbg_printf("i=%d\n",i);
    return i;
}



//����CRC16
static unsigned int calcrc(unsigned char *ptr, unsigned int count)
{
    unsigned int crc = 0;
    while (count--)
    {
        crc =_crc_xmodem_update(crc,*ptr++);
    }
    return crc;
}

static int xmodem_transfer(char *base)
{
    unsigned char c;
    unsigned int i;
    unsigned int crc;
    unsigned int filesize=0;
    unsigned char BlockCount=1;               //���ݿ��ۼ�(��8λ�����뿼�����)

    //��PC�����Ϳ�ʼ��ʾ��Ϣ
        STATUS=ST_WAIT_START;               //��������='d'��'D',����XMODEM
    c=0;
	while(1)
	{
		tgt_putchar(XMODEM_WAIT_CHAR);
		if(testchar()>0)break;

	}
    while(STATUS!=ST_OK)                  //ѭ�����գ�ֱ��ȫ������
    {
	
        i=get_data(&strXMODEM.SOH,BLOCKSIZE+5,1000);   //��ʱ1�룬����133�ֽ�����
        if(i)
        {
            //�������ݰ��ĵ�һ������ SOH/EOT/CAN
            switch(strXMODEM.SOH)
            {
            case XMODEM_SOH:               //�յ���ʼ��SOH
                if (i>=(BLOCKSIZE+5))
                {
                    STATUS=ST_BLOCK_OK;
                }
                else
                {
                    STATUS=ST_BLOCK_FAIL;      //������ݲ��㣬Ҫ���ط���ǰ���ݿ�
                    tgt_putchar(XMODEM_NAK);
                }
                break;
            case XMODEM_EOT:               //�յ�������EOT
                tgt_putchar(XMODEM_ACK);            //֪ͨPC��ȫ���յ�
                STATUS=ST_OK;
            printf("transfer succeed!\n");
                break;
            case XMODEM_CAN:               //�յ�ȡ����CAN
                tgt_putchar(XMODEM_ACK);            //��ӦPC��
                STATUS=ST_OK;
            printf("Warning:user cancelled!\n");
                break;
            default:                     //��ʼ�ֽڴ���
                tgt_putchar(XMODEM_NAK);            //Ҫ���ط���ǰ���ݿ�
                STATUS=ST_BLOCK_FAIL;
                break;
            }
        }
		else 
		{
		dbg_printf("time out!\n");
			break;
		}
        if (STATUS==ST_BLOCK_OK)            //����133�ֽ�OK������ʼ�ֽ���ȷ
        {
			dbg_printf("BlockCount=%d,strXMODEM.BlockNo=%d\n",BlockCount,strXMODEM.BlockNo);
            if (BlockCount != strXMODEM.BlockNo)//�˶����ݿ�����ȷ
            {
                tgt_putchar(XMODEM_NAK);            //���ݿ��Ŵ���Ҫ���ط���ǰ���ݿ�
                continue;
            }
            if (BlockCount !=(unsigned char)(~strXMODEM.nBlockNo))
            {
                tgt_putchar(XMODEM_NAK);            //���ݿ��ŷ������Ҫ���ط���ǰ���ݿ�
                continue;
            }
            crc=strXMODEM.CRC16hi<<8;
            crc+=strXMODEM.CRC16lo;
            //AVR��16λ�����ǵ�λ���ȣ�XMODEM��CRC16�Ǹ�λ����
            if(calcrc(&strXMODEM.Xdata[0],BLOCKSIZE)!=crc)
            {
                tgt_putchar(XMODEM_NAK);              //CRC����Ҫ���ط���ǰ���ݿ�
				dbg_printf("crc error\n");
                continue;
            }
            //��ȷ����128���ֽ����ݣ��պ���M16��һҳ
            memcpy(base+filesize,strXMODEM.Xdata,128);
            filesize+=128;
            tgt_putchar(XMODEM_ACK);                 //��Ӧ����ȷ�յ�һ�����ݿ�
            BlockCount++;                       //���ݿ��ۼƼ�1
        }
    }

    //�˳�Bootloader���򣬴�0x0000��ִ��Ӧ�ó���
    printf("xmodem finished\n");
    return filesize;
}

#if NRAMFILES > 0
struct Ramfile;
struct Ramfile *addRamFile(char *filename, unsigned long base, unsigned long size, int flags);
int deleteRamFile(char *filename);
#endif

static int xmodem(int argc,char *argv[])
{
    int i = 0;
    char buf_2[100];
    char tmp[20];
    int base_flash = 0xbfc00000;
    int start_ram = 0x80300000;
    char boot[20]="elf";
    void *base = NULL;
	char *file;
	int file_size;
#ifdef XMODEM_DEBUG
    pmsg=dbgbuf;
	dbg=0;
#endif
	base = 0x84000000;
	file="xmodem";

	for(i=1;i<argc;i++)
	{
	 if(!strncmp(argv[i],"base=",5))
	 {
	  base=strtoul(&argv[i][5],0,0);
	 }
	 else if(!strncmp(argv[i],"file=",5))
	 {
	  file=&argv[i][5];
	 }
#ifdef XMODEM_DEBUG
	 else if(!strncmp(argv[i],"dbg=",5))
	 {
	  dbg=strtoul(&argv[i][4],0,0);
	 }
#endif
	}


    printf("Waiting for serial transmitting datas\n");
    file_size = xmodem_transfer(base);
    printf("Load successfully! Start at 0x%x, size 0x%x\n", base, file_size);
#if NRAMFILES > 0
	deleteRamFile(file);
	addRamFile(file,base,file_size,0);
#endif
    return 0; 
}


static const Cmd Cmds[] =
{
	{"MyCmds"},
	{"xmodem","",0,"xmodem serial",xmodem,0,99,CMD_REPEAT},
#ifdef XMODEM_DEBUG
	{"dmsg","",0,"xmodem serial",dmsg,0,99,CMD_REPEAT},
#endif
	{0, 0}
};


static void init_cmd __P((void)) __attribute__ ((constructor));

static void
init_cmd()
{
	cmdlist_expand(Cmds, 1);
}

