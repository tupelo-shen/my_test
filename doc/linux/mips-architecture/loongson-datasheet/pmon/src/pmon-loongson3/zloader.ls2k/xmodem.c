//#include <asm/context.h>
//#include <asm/cacheops.h>
//#include <asm/mipsregs.h>
//#include <asm/r4kcache.h>
#define tgt_puts tgt_puts1
#undef XMODEM_DEBUG
static inline __attribute__((always_inline))  int now()
{
int count;
asm volatile("mfc0 %0,$9":"=r"(count)); 
return count;
}

static inline __attribute__((always_inline))  void *memcpy(char *s1, const char *s2, int n)
{
while(n--)
  *s1++=*s2++;
}

static __attribute__ ((section (".data")))  void * memset(void * s,int c, int count)
{
	char *xs = (char *) s;

	while (count--)
		*xs++ = c;

	return s;
}

static __attribute__ ((section (".data")))  unsigned short _crc_xmodem_update (unsigned short crc, unsigned char data)
{
    int i;
    crc = crc ^ ((unsigned short)data << 8);
    for (i=0; i<8; i++)
    {
        if (crc & 0x8000)
            crc = (crc << 1) ^ 0x1021;
        else
            crc <<= 1;
    }
    return crc;
}

static __attribute__ ((section (".data"))) int tgt_puts(char *str)
{
	while(*str)
	tgt_putchar1(*str++);
	return 0;
}



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
} strXMODEM ;                           //XMODEM�Ľ������ݽṹ

/*   GCC�����ַʹ��32λ���ȣ���Ӧ����AVR������*/


#define ST_WAIT_START       0x00         //�ȴ�����
#define ST_BLOCK_OK       0x01         //����һ�����ݿ�ɹ�
#define ST_BLOCK_FAIL       0x02         //����һ�����ݿ�ʧ��
#define ST_OK             0x03         //���


#ifdef XMODEM_DEBUG
#define dbg_printf tgt_puts
#else
#define dbg_printf(...)
#endif

static __attribute__ ((section (".data")))  int testchar()
{
	int count=2;
	int total, start;
	start = now();

	while(1)
	{
    if(tgt_testchar1())
	return 100;
	if(!count)break;
	if((now()-start>0x3000000 )){
	start = now();
	count--;
	}
	}

	   return 0;
}

static __attribute__ ((section (".data")))  int get_data(unsigned char *ptr,unsigned int len,unsigned int timeout)
{
	int i=0;
	volatile int count=1;
	while(i<len)
	{
		if(testchar()>0)
		ptr[i++]=tgt_getchar();
		else {
		if(!count)break;
		count--;
		}
	}
    return i;
}



//����CRC16
static __attribute__ ((section (".data")))  unsigned int calcrc(unsigned char *ptr, unsigned int count)
{
    unsigned int crc = 0;
    while (count--)
    {
        crc =_crc_xmodem_update(crc,*ptr++);
    }
    return crc;
}

static int __attribute__ ((section (".data"))) program1(char *p,int off, int size)
{
	static int first = 1;

	if(first) 
	{
		erase();
		first = 0;
	}

		 program(p,off,size);
}

int __attribute__ ((section (".data"))) xmodem_transfer(char *base)
{
    unsigned char c;
    unsigned int i;
    unsigned int crc;
    unsigned int filesize=0;
    unsigned char BlockCount=1;               //���ݿ��ۼ�(��8λ�����뿼�����)
    unsigned char STATUS;                  //����״̬

	spi_init();

    //��PC�����Ϳ�ʼ��ʾ��Ϣ
        STATUS=ST_WAIT_START;               //��������='d'��'D',����XMODEM
    c=0;
	while(1)
	{
		tgt_putchar1(XMODEM_WAIT_CHAR);
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
                    tgt_putchar1(XMODEM_NAK);
                }
                break;
            case XMODEM_EOT:               //�յ�������EOT
                //tgt_putchar1(XMODEM_ACK);            //֪ͨPC��ȫ���յ�
                //STATUS=ST_OK;
                tgt_putchar1(XMODEM_NAK);            //Ҫ���ط���ǰ���ݿ�
                STATUS=ST_BLOCK_FAIL;
            dbg_printf("transfer succeed!\r\n");
                break;
            case XMODEM_CAN:               //�յ�ȡ����CAN
                //tgt_putchar1(XMODEM_ACK);            //��ӦPC��
                //STATUS=ST_OK;
                tgt_putchar1(XMODEM_NAK);            //Ҫ���ط���ǰ���ݿ�
                STATUS=ST_BLOCK_FAIL;
            dbg_printf("Warning:user cancelled!\r\n");
                break;
            default:                     //��ʼ�ֽڴ���
                tgt_putchar1(XMODEM_NAK);            //Ҫ���ط���ǰ���ݿ�
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
            if (BlockCount != strXMODEM.BlockNo)//�˶����ݿ�����ȷ
            {
                tgt_putchar1(XMODEM_NAK);            //���ݿ��Ŵ���Ҫ���ط���ǰ���ݿ�
                continue;
            }
            if (BlockCount !=(unsigned char)(~strXMODEM.nBlockNo))
            {
                tgt_putchar1(XMODEM_NAK);            //���ݿ��ŷ������Ҫ���ط���ǰ���ݿ�
                continue;
            }
            crc=strXMODEM.CRC16hi<<8;
            crc+=strXMODEM.CRC16lo;
            //AVR��16λ�����ǵ�λ���ȣ�XMODEM��CRC16�Ǹ�λ����
            if(calcrc(&strXMODEM.Xdata[0],BLOCKSIZE)!=crc)
            {
                tgt_putchar1(XMODEM_NAK);              //CRC����Ҫ���ط���ǰ���ݿ�
				dbg_printf("crc error\n");
                continue;
            }


            //��ȷ����128���ֽ����ݣ��պ���M16��һҳ
            memcpy(base+(filesize&0xfff),strXMODEM.Xdata,128);
            filesize+=128;
            tgt_putchar1(XMODEM_ACK);                 //��Ӧ����ȷ�յ�һ�����ݿ�
            BlockCount++;                       //���ݿ��ۼƼ�1
	    if((filesize&0xfff)==0)
		 program1(base,filesize-0x1000,0x1000);
        }
    }
	    if((filesize&0xfff))
		 program1(base,(filesize&~0xfff),0x1000);

    //�˳�Bootloader���򣬴�0x0000��ִ��Ӧ�ó���
    tgt_puts("xmodem finished\r\n");
    return filesize;
}


static char blockdata[0x1000];

int xmodem()
{
int size;
int offset;
char mode;
//clean char buf
tgt_puts("xmodem now\r\n");
while(tgt_getchar()!='x') tgt_puts("press x to tranfser\r\n");

//tgt_puts("haha\r\n");

xmodem_transfer(blockdata);

while(1);

return 0;
}


