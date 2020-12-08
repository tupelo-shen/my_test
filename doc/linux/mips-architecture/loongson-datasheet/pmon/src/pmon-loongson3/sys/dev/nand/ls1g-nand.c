#include<pmon.h>
#include<asm.h>
#include<machine/types.h>
#include<linux/mtd/mtd.h>
#include<linux/mtd/nand.h>
#include<linux/mtd/partitions.h>
#include<sys/malloc.h>

#ifndef __iomem
#define __iomem
#endif

#define DMA_DESP        0x80800000
#define DMA_DESP_ORDER  0x80800008 // bit3 :1 ,means dma start; otherwize,
#define DMA_ASK_ORDER   0x80800004

#define DDR_PHYADDR     0x82400000
#define DDR_PHY         0x82400000
#define DDR_ADDR        0x82400000


#define NAND_BASE           0xbbee0000
#define DMA_ACCESS_ADDR     0x1bee0040
#define ORDER_REG_ADDR      0xbbd00100
#define MAX_BUFF_SIZE	4096
#define PAGE_SHIFT      12
#define NO_SPARE_ADDRH(x)   ((x) >> (32 - (PAGE_SHIFT - 1 )))   
#define NO_SPARE_ADDRL(x)   ((x) << (PAGE_SHIFT - 1))
#define SPARE_ADDRH(x)      ((x) >> (32 - (PAGE_SHIFT )))   
#define SPARE_ADDRL(x)      ((x) << (PAGE_SHIFT ))
#define ALIGN_DMA(x)       (((x)+ 3)/4)
#define CHIP_DELAY_TIMEOUT (2*HZ/10)

#define STATUS_TIME_LOOP_R  300 
#define STATUS_TIME_LOOP_WS  50  
#define STATUS_TIME_LOOP_WM  40  
#define STATUS_TIME_LOOP_E  40 
#define NAND_CMD        0x1
#define NAND_ADDRL      0x2
#define NAND_ADDRH      0x4
#define NAND_TIMING     0x8
#define NAND_IDL        0x10
#define NAND_STATUS_IDL 0x20
#define NAND_PARAM      0x40
#define NAND_OP_NUM     0X80
#define NAND_CS_RDY_MAP 0x100

#define DMA_ORDERAD     0x1
#define DMA_SADDR       0x2
#define DMA_DADDR       0x4
#define DMA_LENGTH      0x8
#define DMA_STEP_LENGTH 0x10
#define DMA_STEP_TIMES  0x20
#define DMA_CMD         0x40


enum{
    ERR_NONE        = 0,
    ERR_DMABUSERR   = -1,
    ERR_SENDCMD     = -2,
    ERR_DBERR       = -3,
    ERR_BBERR       = -4,
};

enum{
    STATE_READY = 0,
    STATE_BUSY  ,
};

struct ls1g_nand_platform_data{
        int enable_arbiter;
        struct mtd_partition *parts;
        unsigned int nr_parts;
};

struct ls1g_nand_cmdset {
        uint32_t    cmd_valid:1;
	uint32_t    read:1;
	uint32_t    write:1;
	uint32_t    erase_one:1;
	uint32_t    erase_con:1;
	uint32_t    read_id:1;
	uint32_t    reset:1;
	uint32_t    read_sr:1;
	uint32_t    op_main:1;
	uint32_t    op_spare:1;
	uint32_t    done:1;
        uint32_t    resv1:5;//11-15 reserved
        uint32_t    nand_rdy:4;//16-19
        uint32_t    nand_ce:4;//20-23
        uint32_t    resv2:8;//24-32 reserved
};

struct ls1g_nand_dma_desc{
        uint32_t    orderad;
        uint32_t    saddr;
        uint32_t    daddr;
        uint32_t    length;
        uint32_t    step_length;
        uint32_t    step_times;
        uint32_t    cmd;
};

struct ls1g_nand_dma_cmd{
        uint32_t    dma_int_mask:1;
        uint32_t    dma_int:1;
        uint32_t    dma_sl_tran_over:1;
        uint32_t    dma_tran_over:1;
        uint32_t    dma_r_state:4;
        uint32_t    dma_w_state:4;
        uint32_t    dma_r_w:1;
        uint32_t    dma_cmd:2;
        uint32_t    revl:17;
};

struct ls1g_nand_desc{
        uint32_t    cmd;
        uint32_t    addrl;
        uint32_t    addrh;
        uint32_t    timing;
        uint32_t    idl;//readonly
        uint32_t    status_idh;//readonly
        uint32_t    param;
        uint32_t    op_num;
        uint32_t    cs_rdy_map;
};

struct ls1g_nand_info {
	struct nand_chip	nand_chip;

//	struct platform_device	    *pdev;
        /* MTD data control*/
	unsigned int 		buf_start;
	unsigned int		buf_count;
        /* NAND registers*/
	void __iomem		*mmio_base;
        struct ls1g_nand_desc   nand_regs;
        unsigned int            nand_addrl;
        unsigned int            nand_addrh;
        unsigned int            nand_timing;
        unsigned int            nand_op_num;
        unsigned int            nand_cs_rdy_map;
        unsigned int            nand_cmd;

	/* DMA information */

        struct ls1g_nand_dma_desc  dma_regs;
        unsigned int            order_reg_addr;  
        unsigned int            dma_orderad;
        unsigned int            dma_saddr;
        unsigned int            dma_daddr;
        unsigned int            dma_length;
        unsigned int            dma_step_length;
        unsigned int            dma_step_times;
        unsigned int            dma_cmd;
        unsigned int		drcmr_dat;//dma descriptor address;
	unsigned int 		drcmr_dat_phys;
        size_t                  drcmr_dat_size;
	unsigned char		*data_buff;//dma data buffer;
	unsigned int 		data_buff_phys;
	size_t			data_buff_size;
        unsigned int            data_ask;
        unsigned int            data_ask_phys;
        unsigned int            data_length;
        unsigned int            cac_size;
        unsigned int            size;
        unsigned int            num;
        
	/* relate to the command */
	unsigned int		state;
//	int			use_ecc;	/* use HW ECC ? */
	size_t			data_size;	/* data size in FIFO */
        unsigned int            cmd;
        unsigned int            cmd_prev;
        unsigned int            page_addr;
//	struct completion 	cmd_complete;
        unsigned int            seqin_column;
        unsigned int            seqin_page_addr;
};

struct ls1g_nand_ask_regs{
        unsigned int dma_order_addr;
        unsigned int dma_mem_addr;
        unsigned int dma_dev_addr;
        unsigned int dma_length;
        unsigned int dma_step_length;
        unsigned int dma_step_times;
        unsigned int dma_state_tmp;
};

static struct mtd_info *ls1g_soc_mtd = NULL;
#if 0
struct mtd_info *_soc_mtd = NULL;
#define KERNEL_AREA_SIZE 32*1024*1024
 const struct mtd_partition partition_info[] = {
//	{name ,size,offset,mask_flags }
        {"kernel",KERNEL_AREA_SIZE,0,0},
        {"os",0,KERNEL_AREA_SIZE,0},
        {(void *)0,0,0,0}
};
#endif
static struct nand_ecclayout hw_largepage_ecclayout = {
	.eccbytes = 24,
	.eccpos = {
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = { {2, 38} }
};

#define show_data_debug  0
#define show_debug(x,y)     show_debug_msk(x,y)
#define show_debug_msk(x,y)   do{ if(show_data_debug) {printk(KERN_ERR "%s:\n",__func__);show_data(x,y);} }while(0)

static void show_data(void * base,int num)
{
    int i=0;
    unsigned char *arry=( unsigned char *) base;
    printk(KERN_ERR "base==0x%08x \n",arry);
    for(i=0;i<num;i++){
        if(!(i % 32)){
            printk(KERN_ERR "\n");
        }
        if(!(i % 16)){
            printk("  ");
        }
        printk("%02x ",arry[i]);
    }
    printk(KERN_ERR "\n");
    
}



static int ls1g_nand_ecc_calculate(struct mtd_info *mtd,
		const uint8_t *dat, uint8_t *ecc_code)
{
	return 0;
}
static int ls1g_nand_ecc_correct(struct mtd_info *mtd,
		uint8_t *dat, uint8_t *read_ecc, uint8_t *calc_ecc)
{
	struct ls1g_nand_info *info = mtd->priv;
	/*
	 * Any error include ERR_SEND_CMD, ERR_DBERR, ERR_BUSERR, we
	 * consider it as a ecc error which will tell the caller the
	 * read fail We have distinguish all the errors, but the
	 * nand_read_ecc only check this function return value
	 */
	return 0;
}

static void ls1g_nand_ecc_hwctl(struct mtd_info *mtd, int mode)
{
	return;
}

static int ls1g_nand_waitfunc(struct mtd_info *mtd, struct nand_chip *this)
{
    //udelay(50);
    return 0;
}

static void ls1g_nand_select_chip(struct mtd_info *mtd, int chip)
{
	return;
}

static int ls1g_nand_dev_ready(struct mtd_info *mtd)
{
	return 1;
}

static void ls1g_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct ls1g_nand_info *info = mtd->priv;
	int i,real_len = min_t(size_t, len, info->buf_count - info->buf_start);

	memcpy(buf, info->data_buff + info->buf_start, real_len);
	show_debug(info->data_buff,0x40);
	info->buf_start += real_len;
}

static u16 ls1g_nand_read_word(struct mtd_info *mtd)
{
	struct ls1g_nand_info *info = mtd->priv;
	u16 retval = 0xFFFF;
	if(!(info->buf_start & 0x1) && info->buf_start < info->buf_count){
		retval = *(u16 *)(info->data_buff + info->buf_start);
	}
	info->buf_start += 2;
	return retval;
}

static uint8_t ls1g_nand_read_byte(struct mtd_info *mtd)
{
	struct ls1g_nand_info *info = mtd->priv;
	char retval = 0xFF;

	if (info->buf_start < info->buf_count)
	/* Has just send a new command? */
		retval = info->data_buff[(info->buf_start)++];
	show_debug(info->data_buff,6);
	return retval;
}

static void ls1g_nand_write_buf(struct mtd_info *mtd,const uint8_t *buf, int len)
{
	int i;
	struct ls1g_nand_info *info = mtd->priv;
	int real_len = min_t(size_t, len, info->buf_count - info->buf_start);
//	info->buf_count = real_len;

	memcpy(info->data_buff + info->buf_start, buf, real_len);
	show_debug(info->data_buff,0x20);
	info->buf_start += real_len;
}

static int ls1g_nand_verify_buf(struct mtd_info *mtd,const uint8_t *buf, int len)
{
	int i=0; 
	struct ls1g_nand_info *info = mtd->priv;
	show_debug(info->data_buff,0x20);
	while(len--){
		if(buf[i++] != ls1g_nand_read_byte(mtd) ){
			printk("?????????????????????????????????????????????????????verify error...\n\n");
			return -1;
		}
	}
	return 0;
}

static void ls1g_nand_cmdfunc(struct mtd_info *mtd, unsigned command,int column, int page_addr);
static void ls1g_nand_init_mtd(struct mtd_info *mtd,struct ls1g_nand_info *info);

int ls1g_nand_init(struct mtd_info *mtd)
{
	int ret=0;
	ret = ls1g_nand_pmon_info_init(mtd->priv, mtd);
	ls1g_nand_init_mtd(mtd, (struct ls1g_nand_info *)(mtd->priv));
	return ret;
}

static void ls1g_nand_init_mtd(struct mtd_info *mtd, struct ls1g_nand_info *info)
{
	struct nand_chip *this = &info->nand_chip;
	
	//(各种芯片选项)在一定程度上设置用于告诉nand_scan函数有关特殊的函数操作.
	this->options = 8;//(f->flash_width == 16) ? NAND_BUSWIDTH_16: 0;

	this->waitfunc		= ls1g_nand_waitfunc;	/*等待设备准备好 硬件相关函数*/
	this->select_chip		= ls1g_nand_select_chip;	/*控制CE信号*/
	this->dev_ready		= ls1g_nand_dev_ready;	/*板特定的设备ready/busy信息*/
	this->cmdfunc			= ls1g_nand_cmdfunc;		/*命令处理函数*/
	this->read_word		= ls1g_nand_read_word;	/*从芯片读一个字*/
	this->read_byte		= ls1g_nand_read_byte;	/*从芯片读一个字节*/
	this->read_buf		= ls1g_nand_read_buf;	/*将芯片数据读到缓冲区*/
	this->write_buf		= ls1g_nand_write_buf;	/*将缓冲区内容写入芯片*/
	this->verify_buf		= ls1g_nand_verify_buf;	/*验证芯片和写入缓冲区中的数据*/

//	this->ecc.mode		= NAND_ECC_NONE;
	this->ecc.mode		= NAND_ECC_SOFT;	/*ECC模式 这里是软件模式*/
//	this->ecc.hwctl		= ls1g_nand_ecc_hwctl;
//	this->ecc.calculate	= ls1g_nand_ecc_calculate;
//	this->ecc.correct		= ls1g_nand_ecc_correct;
//	this->ecc.size		= 2048;
//	this->ecc.bytes		= 24;

//	this->ecc.layout		= &hw_largepage_ecclayout;
//	mtd->owner				= THIS_MODULE;
}

#define write_z_cmd  do{                                    \
            *((volatile unsigned int *)(0xbbee0000)) = 0;   \
            *((volatile unsigned int *)(0xbbee0000)) = 0;   \
            *((volatile unsigned int *)(0xbbee0000)) = 400; \
    }while(0)


static unsigned ls1g_nand_status(struct ls1g_nand_info *info)
{
    struct ls1g_nand_desc *nand_regs = (volatile struct ls1g_nand_desc *)(info->mmio_base);
    struct ls1g_nand_cmdset *nand_cmd = (struct ls1g_nand_cmdset *)(&(nand_regs->cmd));
    udelay(100);
    return(nand_cmd->done);
}

/*
 *  flags & 0x1   orderad
 *  flags & 0x2   saddr
 *  flags & 0x4   daddr
 *  flags & 0x8   length
 *  flags & 0x10  step_length
 *  flags & 0x20  step_times
 *  flags & 0x40  cmd
 ***/
static void show_dma_regs(void *dma_regs,int flag)
{
    unsigned int *regs=dma_regs;
    printf("dump dma register\n");
    printf("0x%08x:0x%08x\n",regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);

    regs=0xbbee0000;
    printf("dump nand register\n");
    printf("0x%08x:0x%08x\n",regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
    printf("0x%08x:0x%08x\n",++regs,*regs);
 
//    if(flag)
    printf("ORDER_REG_ADDR:0x%08x\n",*(volatile unsigned int *)ORDER_REG_ADDR);
}
/*before DMA rw=1(hit writeback and invalidated)  ;after DMA rw=0(invalidated)*/
#define  __dma_write_ddr2(cmd,bit) (~((cmd)&(1<<bit)))
static void dma_cache_nand(struct ls1g_nand_info *info,unsigned char rw )
{
    struct ls1g_nand_dma_desc *dma_base = (volatile struct ls1g_nand_dma_desc *)(info->drcmr_dat);

    if(__dma_write_ddr2(dma_base->cmd,12))
    {
//        CPU_IOFlushDCache(((dma_base->saddr)&0x1fffffff)|0x80000000,(dma_base->length)*4,rw);
    }
    
}

static void nand_cache_inv(unsigned long base,unsigned long num)
{
    
    CPU_IOFlushDCache((base & 0x1fffffff)|0x80000000,num,0);
}

static void nand_cache_wb(unsigned long base,unsigned long num)
{
    CPU_IOFlushDCache((base & 0x1fffffff)|0x80000000,num,1);
}

static void dma_setup(unsigned int flags,struct ls1g_nand_info *info)
{
	int order;
	struct ls1g_nand_dma_desc *dma_base = (volatile struct ls1g_nand_dma_desc *)(info->drcmr_dat);
	dma_base->orderad = (flags & DMA_ORDERAD)== DMA_ORDERAD ? info->dma_regs.orderad : info->dma_orderad;
	dma_base->saddr = (flags & DMA_SADDR)== DMA_SADDR ? info->dma_regs.saddr : info->dma_saddr;
	dma_base->daddr = (flags & DMA_DADDR)== DMA_DADDR ? info->dma_regs.daddr : info->dma_daddr;
	dma_base->length = (flags & DMA_LENGTH)== DMA_LENGTH ? info->dma_regs.length: info->dma_length;
	info->data_length = info->dma_regs.length;
	dma_base->step_length = (flags & DMA_STEP_LENGTH)== DMA_STEP_LENGTH ? info->dma_regs.step_length: info->dma_step_length;
	dma_base->step_times = (flags & DMA_STEP_TIMES)== DMA_STEP_TIMES ? info->dma_regs.step_times: info->dma_step_times;
	dma_base->cmd = (flags & DMA_CMD)== DMA_CMD ? info->dma_regs.cmd: info->dma_cmd;
	/*flush cache before DMA operation*/
	if((dma_base->cmd)&(0x1 << 12)){
		//nand_cache_wb((unsigned long)(info->data_buff),info->cac_size);
	}
	//nand_cache_wb((unsigned long)(info->drcmr_dat),0x20);

	//printf("dump dma descriptor after dma set up \n"); 
	//printf("orderad = %08x, saddr = %08x, daddr = %08x, length = %08x, setup_length = %08x, cmd = %08x\n", dma_base->orderad,  dma_base->saddr, dma_base->daddr , dma_base->length, dma_base->step_length, dma_base->cmd);

	*(volatile unsigned int *)(info->order_reg_addr) = ((unsigned int )info->drcmr_dat_phys) | 0x1<<3;
//	memset(&(info->dma_regs),0,sizeof(struct ls1g_nand_dma_desc));
}

static void dma_ask(struct ls1g_nand_info *info)
{
	memset((char *)info->data_ask,0,sizeof(struct ls1g_nand_ask_regs));
	*(volatile unsigned int *)info->order_reg_addr = 0x1<<2|( (info->data_ask_phys)& 0xfffffff0) | 0x80000000;
	//show_dma_regs((void *)(info->data_ask),1);
}

/**
 *  flags & 0x1     cmd
 *  flags & 0x2     addrl
 *  flags & 0x4     addrh
 *  flags & 0x8     timing
 *  flags & 0x10    idl
 *  flags & 0x20    status_idh
 *  flags & 0x40    param
 *  flags & 0x80    op_num
 *  flags & 0x100   cs_rdy_map
 ****/
static void nand_setup(unsigned int flags ,struct ls1g_nand_info *info)
{
//	printk("addrl+++++++++++++++++++++==%x\n\n",info->nand_regs.addrl);
	int i; 
	struct ls1g_nand_desc *nand_base = (struct ls1g_nand_desc *)(info->mmio_base);
	nand_base->cmd = 0;
	nand_base->cmd = 0x40; // reset
	udelay(500);

	nand_base->addrl = (flags & NAND_ADDRL)==NAND_ADDRL ? info->nand_regs.addrl: info->nand_addrl;
	nand_base->addrh = (flags & NAND_ADDRH)==NAND_ADDRH ? info->nand_regs.addrh: info->nand_addrh;
	//nand_base->timing = (flags & NAND_TIMING)==NAND_TIMING ? info->nand_regs.timing: info->nand_timing;
	nand_base->op_num = (flags & NAND_OP_NUM)==NAND_OP_NUM ? info->nand_regs.op_num: info->nand_op_num;	//NAND读写操作Byte数；擦除为块数
	nand_base->cs_rdy_map = (flags & NAND_CS_RDY_MAP)==NAND_CS_RDY_MAP ? info->nand_regs.cs_rdy_map: info->nand_cs_rdy_map;
	if(flags & NAND_CMD){
		nand_base->cmd = (info->nand_regs.cmd) &(~0xff);
		nand_base->cmd = info->nand_regs.cmd;
		//printf("nand cmd: %08x, nand op_num = %08x, info op_num = %08x\n", nand_base->cmd, info->nand_regs.op_num, info->nand_op_num);
		//printf("info->nand_regs.addrl: %08x\t info->nand_regs.addrh: %08x\n", info->nand_regs.addrl, info->nand_regs.addrh);
		if(info->nand_regs.cmd & 0x20){
			i = 100;
			//判断done操作完成 nand_cmd 0xbbee0000
			while(!ls1g_nand_status(info)){
				if(!(i--)){
					write_z_cmd; //写操作
					break;
				}
				udelay(100);
			}
			*(int *)(info->data_buff) = nand_base->idl;
		}
	}
	else
		nand_base->cmd = info->nand_cmd;

	//printf("cmd: %08x, naddrl: %08x addrh: %08x\n",nand_base->cmd, nand_base->addrl, nand_base->addrh);
}

static  int sync_dma(struct ls1g_nand_info *info)
{
	int *end ;
	struct ls1g_nand_ask_regs *ask = info->data_ask;

	//end = ((unsigned int)(info->data_buff_phys)&0x1fffffff) + info->data_length*4;
	end = ((unsigned int)(info->data_buff_phys)) + info->data_length*4;

	
#if 1
	//printf("sync length ...\n");
	dma_ask(info);
#if 0
	while(1){
		udelay(100);
		if(ask->dma_mem_addr == end)
			break;
		 printf("ask->dma_mem_addr = %08x, end = %08x\n", ask->dma_mem_addr, end);
	}
#endif
	//printf("sync dma step1 Over!\n");
	while((*((unsigned int*) (NAND_BASE)) & (0x1<<10)) == 0){
	  udelay(500);
	  //printf("sync dma: cmd = %08x ...\n", *((unsigned int*) (NAND_BASE)));
	}
	
#endif
	/*flush cache after DMA operation*/
//	dma_cache_nand(info,0);
	return 0;
}

static void ls1g_nand_cmdfunc(struct mtd_info *mtd, unsigned command,int column, int page_addr)
{
	struct ls1g_nand_info *info = mtd->priv;
	int ret,i,nandcmd;
	unsigned cmd_prev;
	int status_time,page_prev;
//        int timeout = CHIP_DELAY_TIMEOUT;
	unsigned int base;
//        init_completion(&info->cmd_complete);
	static int ccc=0;
	
	cmd_prev = info->cmd;
	page_prev = info->page_addr;
	info->cmd = command;
	info->page_addr = page_addr;
	//show_dma_regs((void *)(info->mmio_base),0); 
	switch(command){
		case NAND_CMD_READOOB:
			//printf("NAND_CMD_READOOB: page = %08x\n", page_addr);
			if(info->state == STATE_BUSY){
				printk("nandflash chip if busy...\n");
				return;
			}
			info->state = STATE_BUSY;
			info->buf_count = mtd->oobsize;
			info->buf_start = 0;
			info->cac_size = info->buf_count;
			if(info->buf_count <=0 )
				break;
			/*nand regs set*/
			info->dma_regs.cmd = 0;		//下一个描述符地址寄存器
			info->nand_regs.cmd = 0;		//
#if 0
			info->nand_regs.addrh = SPARE_ADDRH(page_addr);	//读、写、擦除操作起始地址高8位
			info->nand_regs.addrl = SPARE_ADDRL(page_addr) + mtd->writesize;	//读、写、擦除操作起始地址低32位
#else
			info->nand_regs.addrh = page_addr;	//读、写、擦除操作起始地址高8位
			info->nand_regs.addrl = 0x800;	//读、写、擦除操作起始地址低32位
#endif
			info->nand_regs.op_num = info->buf_count;	//NAND读写操作Byte数；擦除为块数
			/*nand cmd set */ 
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->read = 1;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->op_spare = 1;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->cmd_valid = 1;
			/*dma regs config*/
			info->dma_regs.length = ALIGN_DMA(info->buf_count);	//传输数据长度寄存器 代表一块被搬运内容的长度，单位是字
			((struct ls1g_nand_dma_cmd *)&(info->dma_regs.cmd))->dma_int_mask = 0;
			/*dma GO set*/       
			nand_setup(NAND_ADDRL|NAND_ADDRH|NAND_OP_NUM|NAND_CMD, info);
			dma_setup(DMA_LENGTH|DMA_CMD, info);
			//printf("first sync dma\n");
			sync_dma(info);
		  	//show_dma_regs((void *)(info->data_ask),1); // by xqch
		break;
	#if 0            
		case NAND_CMD_READOOB:
		info->state = STATE_BUSY; 
		info->buf_count = mtd->oobsize - column;
		info->buf_start = 0;
		if(info->buf_count <=0 )
		break;
		/*nand regs set*/
		info->nand_regs.addrh =  page_addr >> (32 - PAGE_SHIFT);

		info->nand_regs.addrl = (page_addr << PAGE_SHIFT) + column + 2048;
		info->nand_regs.op_num = info->buf_count;
		/*nand cmd set */
		info->nand_regs.cmd=0;
		((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->read = 1;
		((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->op_spare = 1;
		((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->cmd_valid = 1;
		/*dma regs config*/
		info->dma_regs.length =0;
		info->dma_regs.cmd = 0;
		info->dma_regs.length = (info->buf_count + 3)/4;
		((struct ls1g_nand_dma_cmd *)&(info->dma_regs.cmd))->dma_int_mask = 0;
		/*dma GO set*/       
		dma_setup(DMA_LENGTH|DMA_CMD,info);
		nand_setup(NAND_ADDRL|NAND_ADDRH|NAND_OP_NUM|NAND_CMD,info);
		//                printf("\ncmdfunc\n");
		sync_dma(info);
		break;
	#endif     
		case NAND_CMD_READ0:
			//printf("NAND_CMD_READ0: page = %08x\n", page_addr);
			if(info->state == STATE_BUSY){
				printk("nandflash chip if busy...\n");
				return;
			}
			info->state = STATE_BUSY;
			info->buf_start =  0 ;
			info->cac_size = info->buf_count;
			if(info->buf_count <=0 )
				break;
#if 0
			info->buf_count = mtd->oobsize + mtd->writesize ;
			info->nand_regs.addrh = SPARE_ADDRH(page_addr);
			info->nand_regs.addrl = SPARE_ADDRL(page_addr);
#else
			//info->buf_count = mtd->writesize ;
			info->buf_count = mtd->oobsize + mtd->writesize ;
			info->nand_regs.addrh = page_addr;
			info->nand_regs.addrl = 0x000;
#endif
			info->nand_regs.op_num = info->buf_count;
			/*nand cmd set */ 
			info->nand_regs.cmd = 0; 
			info->dma_regs.cmd = 0;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->read = 1;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->op_spare = 1;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->op_main = 1;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->cmd_valid = 1; 
			/*dma regs config*/
			info->dma_regs.length = ALIGN_DMA(info->buf_count);
			((struct ls1g_nand_dma_cmd *)&(info->dma_regs.cmd))->dma_int_mask = 0;
			dma_setup(DMA_LENGTH|DMA_CMD,info);
			nand_setup(NAND_ADDRL|NAND_ADDRH|NAND_OP_NUM|NAND_CMD,info);

		  	//show_dma_regs((void *)(info->data_ask),1); // by xqch
			sync_dma(info);
		break;
		case NAND_CMD_SEQIN:
			if(info->state == STATE_BUSY){
				printk("nandflash chip if busy...\n");
				return;
			}
			info->state = STATE_BUSY;
			info->buf_count = mtd->oobsize + mtd->writesize - column;
			info->buf_start = 0;
			info->seqin_column = column;

			info->seqin_page_addr = page_addr;
			//                complete(&info->cmd_complete);
			//printf("NAND_CMD_SEQIN: column =%08x, page_addr = %08x\n", column, page_addr);
		break;
		case NAND_CMD_PAGEPROG:
			//printf("NAND_CMD_PAGEPROG: column =%08x, page_addr = %08x\n", column, page_addr);
			if(info->state == STATE_BUSY){
				printf("nandflash chip if busy...\n");
				return;
			}
			info->state = STATE_BUSY;
			if(cmd_prev != NAND_CMD_SEQIN){
				printf("Prev cmd don't complete...\n");
				break;
			}
			if(info->buf_count <= 0 )
				break;

			if(((info->num)++) % 512 == 0){
				printk("nand have write : %d M\n",(info->size)++); 
			}

			/*nand regs set*/

			//info->buf_count = mtd->writesize ;
			//printf("NAND_CMD_PAGEPROG: column = %08x\n", column);
			//info->buf_count = mtd->oobsize + mtd->writesize - column;
			info->buf_count = mtd->oobsize + mtd->writesize;
			//printf("NAND_CMD_PAGEPROG: buf_count = %08x\n", info->buf_count);
#if 0
			info->nand_regs.addrh =  SPARE_ADDRH(info->seqin_page_addr);
			info->nand_regs.addrl =  SPARE_ADDRL(info->seqin_page_addr) + info->seqin_column;
#else
			info->nand_regs.addrl = info->seqin_column;
			info->nand_regs.addrh = info->seqin_page_addr;
#endif
			info->nand_regs.op_num = info->buf_start;

			/*nand cmd set */ 
			info->nand_regs.cmd = 0; 
			info->dma_regs.cmd = 0;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->write = 1;
			if(info->seqin_column < mtd->writesize)
				((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->op_main = 1;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->op_spare = 1;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->cmd_valid = 1; 

			/*dma regs config*/
			info->dma_regs.length = ALIGN_DMA(info->buf_count);
			((struct ls1g_nand_dma_cmd *)&(info->dma_regs.cmd))->dma_int_mask = 0;
			((struct ls1g_nand_dma_cmd *)&(info->dma_regs.cmd))->dma_r_w = 1;
			nand_setup(NAND_ADDRL|NAND_ADDRH|NAND_OP_NUM|NAND_CMD,info);
			dma_setup(DMA_LENGTH|DMA_CMD,info);
			sync_dma(info);
		break;
		case NAND_CMD_RESET:
			info->state = STATE_BUSY;
			/*nand cmd set */ 
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->reset = 1;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->cmd_valid = 1; 
			nand_setup(NAND_CMD,info);
			status_time = STATUS_TIME_LOOP_R;
			while(!ls1g_nand_status(info)){
				/*
				if(!(status_time--)){
					write_z_cmd;
					break;
				}
				*/
				udelay(50);
			}

			info->state = STATE_READY;
			//                complete(&info->cmd_complete);
		break;
		case NAND_CMD_ERASE1:
			//printf("NAND_CMD_ERASE1: page = %08x\n", page_addr);
			info->state = STATE_BUSY;
			/*nand regs set*/
			info->nand_regs.addrl =  0x00;
			info->nand_regs.addrh =  page_addr ;
			/*nand cmd set */ 
			info->nand_regs.cmd = 0; 
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->erase_one = 1;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->cmd_valid = 1; 
			nand_setup(NAND_ADDRL|NAND_ADDRH|NAND_OP_NUM|NAND_CMD,info);
			status_time = STATUS_TIME_LOOP_E;
			udelay(300);    
			while(!ls1g_nand_status(info)){
			  /*
				if(!(status_time--)){
					write_z_cmd;
					break;
				}
			  */
				udelay(50);
			}

			info->state = STATE_READY;
			//                complete(&info->cmd_complete);
		break;
		case NAND_CMD_STATUS:
			info->buf_count = 0x1;
			info->buf_start = 0x0;
			*(unsigned char *)info->data_buff=ls1g_nand_status(info) | 0x80;
			//                complete(&info->cmd_complete);
		break;
		case NAND_CMD_READID:
			if(info->state == STATE_BUSY){
				printf("nandflash chip if busy...\n");
				return;
			}
			info->state = STATE_BUSY;
			info->buf_count = 0x4;
			info->buf_start = 0;
			{
			#define  _NAND_IDL      ( *((volatile unsigned int*)(0xbbee0010)))
			#define  _NAND_IDH       (*((volatile unsigned int*)(0xbbee0014)))
			#define  _NAND_BASE      0xbbee0000
			#define  _NAND_SET_REG(x,y)   do{*((volatile unsigned int*)(_NAND_BASE+x)) = (y);}while(0)                           
			#define  _NAND_READ_REG(x,y)  do{(y) =  *((volatile unsigned int*)(_NAND_BASE+x));}while(0) 

			unsigned int id_val_l=0,id_val_h=0;
			unsigned int timing = 0;
			unsigned char *data = (unsigned char *)(info->data_buff);
			_NAND_READ_REG(0xc,timing);
			_NAND_SET_REG(0xc,0x30f0); 
			_NAND_SET_REG(0x0,0x21); 

			while(((id_val_l |= _NAND_IDL) & 0xff)  == 0){
				id_val_h = _NAND_IDH;
			}

			//printk("id_val_l=0x%08x\nid_val_h=0x%08x\n",id_val_l,id_val_h);
			_NAND_SET_REG(0xc,timing);
			data[0]  = (id_val_h & 0xff);
			data[1]  = (id_val_l & 0xff000000)>>24;
			data[2]  = (id_val_l & 0x00ff0000)>>16;
			data[3]  = (id_val_l & 0x0000ff00)>>8;
			//printk(KERN_ERR "IDS=============================0x%x\n",*((int *)(info->data_buff)));

			}

		#if 0 
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->read_id = 1;
			((struct ls1g_nand_cmdset*)&(info->nand_regs.cmd))->cmd_valid = 1;             
			nand_setup(NAND_CMD,info);
			status_time = STATUS_TIME_LOOP_R;
			while(!ls1g_nand_status(info)){
			if(!(status_time--)){
			write_z_cmd;
			break;
			}
			//udelay(30);
			}
		#endif
		break;
		case NAND_CMD_ERASE2:
		case NAND_CMD_READ1:
		break;
		default :
			printf(KERN_ERR "non-supported command.\n");
		break;
	}

	if(info->cmd == NAND_CMD_READ0 || info->cmd == NAND_CMD_READOOB ){
		//nand_cache_inv((unsigned long)(info->data_buff),info->cac_size);
	}

	info->state = STATE_READY;
}

int ls1g_nand_detect(struct mtd_info *mtd)
{
        printf("NANDFlash info:\nerasesize\t%d B\nwritesize\t%d B\noobsize  \t%d B\n",mtd->erasesize, mtd->writesize,mtd->oobsize );
        return (mtd->erasesize != 1<<17 || mtd->writesize != 1<<11 || mtd->oobsize != 1<<6);

}
static void ls1g_nand_init_info(struct ls1g_nand_info *info)
{
    //*((volatile unsigned int *)0xbbee0018) = 0x30000;	//外部颗粒容量大小
    info->num=0;
    info->size=0;
    info->cac_size = 0; 
    info->state = STATE_READY;

    info->cmd_prev = -1;
    info->page_addr = -1;
    info->nand_addrl = 0x0;
    info->nand_addrh = 0x0;
    //info->nand_timing = 0x4<<8 | 0x12;
    info->nand_op_num = 0x0;
    info->nand_cs_rdy_map = 0x00000000;
    info->nand_cmd = 0;

    info->dma_orderad = 0x80811000;
    info->dma_saddr = info->data_buff_phys;
    info->dma_daddr = DMA_ACCESS_ADDR;
    info->dma_length = 0x0;
    info->dma_step_length = 0x0;
    info->dma_step_times = 0x1;
    info->dma_cmd = 0x0;

    info->order_reg_addr = ORDER_REG_ADDR;
}

int ls1g_nand_pmon_info_init(struct ls1g_nand_info *info, struct mtd_info *mtd)
{
	info->drcmr_dat =  ((unsigned int)(malloc(sizeof(struct ls1g_nand_dma_desc),M_DMAMAP,M_WAITOK))&0x1fffffff) | 0x80000000;
	info->drcmr_dat = 0x82000000;	//DMA描述符地址
	info->drcmr_dat = (unsigned int)((info->drcmr_dat + 15) & ~0xff);	//DMA描述符地址
	if(info->drcmr_dat == NULL)
		return -1;
	info->drcmr_dat_phys = ((info->drcmr_dat) & 0x1fffffff) | 0x80000000;	//DMA描述符物理地址

	info->mmio_base = 0x1bee0000 | 0xa0000000; //NAND寄存器基地址

	//info->data_buff = ((unsigned int)(malloc(MAX_BUFF_SIZE,M_DMAMAP,M_WAITOK))&0x1fffffff)|0x80000000;
	info->data_buff = 0x8d000000;
	if(info->data_buff == NULL)
		return -1;
	printf("data_buff==0x%08x\n",info->data_buff);
	info->data_buff_phys = ((unsigned int)(info->data_buff) & 0x1fffffff) | 0x80000000;	//DAM数据缓存物理地址
	printf("data_buff_phys==0x%08x\n",info->data_buff_phys);

//	info->data_ask = ((unsigned int)(malloc(sizeof(struct ls1g_nand_ask_regs),M_DMAMAP,M_WAITOK))&0x1fffffff)|0xa0000000;
	info->data_ask = 0x82400000;//malloc(sizeof(struct ls1g_nand_ask_regs),M_DMAMAP,M_WAITOK); // address for dma descriptor
	if(info->data_ask ==NULL)
		return -1;
	info->data_ask_phys = ( info->data_ask & 0x1fffffff) |0x80000000;

	ls1g_nand_init_info(info);
	/*
	if(ls1g_nand_detect(mtd)){
		printk(KERN_ERR "PMON driver don't support the NANDFlash!\n");
		return -1;
	}
	*/
	return 0;
}

static void find_good_part(struct mtd_info *ls1g_soc_mtd)
{
	int offs;
	int start=-1;
	char name[20];
	int idx=0;
	
	for(offs=0;offs< ls1g_soc_mtd->size;offs+=ls1g_soc_mtd->erasesize){
		if(ls1g_soc_mtd->block_isbad(ls1g_soc_mtd,offs)&& start>=0){
			sprintf(name,"g%d",idx++);
			add_mtd_device(ls1g_soc_mtd,start,offs-start,name);
			start=-1;
		}
		else if(start<0){
			start=offs;
		}
	}

	if(start>=0){
		sprintf(name,"g%d",idx++);
		add_mtd_device(ls1g_soc_mtd,start,offs-start,name);
	}
}

#define __ww(addr,val)  *((volatile unsigned int*)(addr)) = (val)
#define __rw(addr,val)  val =  *((volatile unsigned int*)(addr))
#define __display(addr,num) do{for(i=0;i<num;i++){printf("0x%08x:0x%08x\n",(addr+i*sizeof(int)),*((volatile unsigned int*)(addr+i*sizeof(int))));}}while(0)
#define NAND_REG_BASE   0xbbee0000
#define NAND_DEV        0x1bee0040

#define STATUS_TIME 100

#define cmd_to_zero  do{            \
            __ww(NAND_REG_BASE,0);  \
            __ww(NAND_REG_BASE,0);  \
            __ww(NAND_REG_BASE,0x400);  \
}while(0)

//验证擦除
unsigned int verify_erase(unsigned int addr,int all)
{
	int i=0,flag=0;
	volatile unsigned int *base = (unsigned int*)DDR_ADDR;
	unsigned int val=0;
	
	//#define DMA_DESP        0xa0800000
	__ww(DMA_DESP+0, 0);				//下一个描述符地址寄存器 这里设置为无效
	__ww(DMA_DESP+0x4, DDR_PHY);	//内存地址寄存器
	__ww(DMA_DESP+0x8, NAND_DEV);	//设备地址寄存器
	__ww(DMA_DESP+0xc, 0x8400);		//长度寄存器 代表一块被搬运内容的长度，单位是字
	__ww(DMA_DESP+0x10, 0x0);		//间隔长度寄存器 间隔长度说明两块被搬运内存数据块之间的长度，前一个step的结束地址与后一个step的开始地址之间的间隔
	__ww(DMA_DESP+0x14, 0x1);		//循环次数寄存器 循环次数说明在一次DMA操作中需要搬运的块的数目
	__ww(DMA_DESP+0x18, 0x0);		//控制寄存器
	__ww(ORDER_REG_ADDR, DMA_DESP_ORDER);	//DMA模块控制寄存器位 在confreg模块，存放第一个DMA描述符地址寄存器
											//#define DMA_DESP_ORDER  0x00800008
											//可以开始读描述符链的第一个DMA描述符

	__ww(NAND_REG_BASE+0x0, 0x0);
	__ww(NAND_REG_BASE+0x4, addr<<(12+6));//读、写、擦除操作起始地址低32位
	__ww(NAND_REG_BASE+0x1c, 0x21000);//main + spare NAND读写操作Byte数；擦除为块数
	__ww(NAND_REG_BASE+0x0, 0x300);	//操作发生在NAND的SPARE区 操作发生在NAND的MAIN区
	__ww(NAND_REG_BASE+0x0, 0x303);	//读操作 命令有效

	//udelay(5000);
	while(1){
		//#define DMA_ASK_ORDER   0x00800004
		__ww(ORDER_REG_ADDR, DMA_ASK_ORDER);//用户请求将当前DMA操作的相关信息写回到指定的内存地址
		__rw(DMA_DESP+0x4, val);	//读取DMA内存的值到val中
		if(val == (0x21000+DDR_PHY)){break;}
		if((*((volatile unsigned int *)(NAND_REG_BASE)) & 0x400) != 0)	break;

		//udelay(400);
	}
	for(;i<0x8400;i++){
		if(*(base+i) != 0xffffffff){printf("\naddr 0x%08x: 0x%08x isn't 0xFFFFFFFF",(base+i),*(base+i));flag=1;if(all){return flag;}}
	}
	return flag;
}

void nandwrite(int argc,char **argv)/*cmd addr(L,page num) timing op_num(byte) */
{
    
	unsigned int cmd, val, pagenum, column;
	unsigned int buf;
	unsigned int addr; // base on byte

	cmd    =  0x05;
	addr = strtoul(argv[1],0,0);
	val = strtoul(argv[2],0,0);
	printf("addr : %08lx, val = %08x\n", addr, val);
	pagenum = addr/2048;
	column = addr % 2048;
	
     __ww(NAND_REG_BASE+0x0,0x0);
	__ww(NAND_REG_BASE+0x4,column);     // set address in a page; no use in erase operation
    __ww(NAND_REG_BASE+0x8,pagenum);     // set address index pages
	__ww(NAND_REG_BASE+0x1c,4);   // set block number(erase unit) (204,188)
//	__ww(NAND_REG_BASE+0x0,cmd);

	/*	dma configure */
	__ww(DDR_PHYADDR,val);
    __ww(DMA_DESP,0xa0081100);  // means next descriptor address, not used
    __ww(DMA_DESP+0x4,DDR_PHYADDR); 
    __ww(DMA_DESP+0x8,NAND_DEV); 
    __ww(DMA_DESP+0xc,1);
    __ww(DMA_DESP+0x10,0x0); 
    __ww(DMA_DESP+0x14,0x1); 
    __ww(DMA_DESP+0x18,0x1000); // bit 12 == 1'b1: read ddr to write dev;bit 12 = 1'b0, read dev to write ddr
	__ww(NAND_REG_BASE+0x0,cmd);
    __ww(ORDER_REG_ADDR,DMA_DESP_ORDER); // start dma

	buf = 0xaabbccdd;
	val = 0x0;
    __rw(NAND_REG_BASE+0x0,val);
	while( (val & 0x400) != 0x400 ) // operation done
	{
		//__rw(NAND_REG_BASE+0x0040,buf);
		//__rw(DDR_PHYADDR,buf);
		__rw(NAND_REG_BASE,val);
		//printf("cmd: %08x, buf data :%08x\n", val, buf);
    }
	//__rw(DDR_PHYADDR,buf);
	//__rw(NAND_REG_BASE,val);
	printf("nand address: %08lx @ page %08x: %08x\n", addr, pagenum,column);

}

void nandread(int argc,char **argv)/*cmd addr(L,page num) timing op_num(byte) */
{
    
	unsigned int cmd, val, pagenum, column;
	unsigned int buf;
	unsigned int addr; // base on byte

	cmd    =  0x03;
	addr = strtoul(argv[1],0,0);
	printf("addr : %08lx\n", addr);
	pagenum = addr/2048;
	column = addr % 2048;
	
     __ww(NAND_REG_BASE+0x0,0x0);
	__ww(NAND_REG_BASE+0x4,column);     // set address in a page; no use in erase operation
    __ww(NAND_REG_BASE+0x8,pagenum);     // set address index pages
	__ww(NAND_REG_BASE+0x1c,4);   // set block number(erase unit) (204,188)
//	__ww(NAND_REG_BASE+0x0,cmd);

	/*	dma configure */
    __ww(DMA_DESP,0xa0081100);  // means next descriptor address, not used
    __ww(DMA_DESP+0x4,DDR_PHYADDR); 
    __ww(DMA_DESP+0x8,NAND_DEV); 
    __ww(DMA_DESP+0xc,1);
    __ww(DMA_DESP+0x10,0x0); 
    __ww(DMA_DESP+0x14,0x1); 
    __ww(DMA_DESP+0x18,0x0000); // bit 12 == 1'b1: read ddr write dev;bit 12 = 1'b0, read dev to write ddr
	__ww(NAND_REG_BASE+0x0,cmd);
    __ww(ORDER_REG_ADDR,DMA_DESP_ORDER); // start dma

	buf = 0xaabbccdd;
	val = 0x0;
    __rw(NAND_REG_BASE+0x0,val);
	while( (val & 0x400) != 0x400 ) // operation done
	{
		//__rw(NAND_REG_BASE+0x0040,buf);
		__rw(DDR_PHYADDR,buf);
		__rw(NAND_REG_BASE,val);
		//printf("cmd: %08x, buf data :%08x\n", val, buf);
    }
		__rw(DDR_PHYADDR,buf);
		__rw(NAND_REG_BASE,val);
	printf("nand address: %08lx @ page %08x: %08x ======  %08x\n", addr, pagenum,column, buf);

}



void nanderase_verify(int argc, char ** argv)
{
	int i=0, flag=0;
	int status_time;
	char *detail;
//	all =strtoul(argv[1],0,0);
	detail = argv[1];
	flag = strncmp(detail, "detail", 6);
//	__ww(0xbfd00420,0x0a000000);		//lxy
	__ww(NAND_REG_BASE+0x0, 0x0);
	__ww(NAND_REG_BASE+0x0, 0x41);	//NAND复位，命令有效
	__ww(NAND_REG_BASE+0x4, 0x00);	//读、写、擦除操作起始地址低32位
	status_time = STATUS_TIME;

	printf("erase blockaddr: 0x%08x\n", i);
	for(i=0; i<1024; i++){
		printf("\b\b\b\b\b\b\b\b");
		printf("%08x", i<<(11+6));
		__ww(NAND_REG_BASE+0x4, i<<(11+6));	//128K？
		__ww(NAND_REG_BASE+0x0, 0x8);	//擦除操作
		__ww(NAND_REG_BASE+0x0, 0x9);	//擦除操作 命令有效
		//udelay(2000);
		//外部NAND芯片RDY情况
		//while((*((volatile unsigned int *)(NAND_REG_BASE)) & 0x1<<16) == 0){
		while((*((volatile unsigned int *)(NAND_REG_BASE)) & 0x400) == 0){
		/*
			if(!(status_time--)){
				cmd_to_zero;	//操作完成
				status_time = STATUS_TIME;
				break;
			}
		*/
			//udelay(80);
		}
		//验证擦除
		if(verify_erase(i,flag)){printf("BLOCK:%d,addr:0x%08x,some error or bad block\n", i, i<<(11+6));}
	}
	printf("\nerase all nandflash ok...\n");

#if 0  
//	__ww(NAND_REG_BASE+0x0,0x9);
//	//udelay(1000000);
//	__display(NAND_REG_BASE,0x10);
//    return 0;  

//    __ww(NAND_REG_BASE+0x0,0x0);
//    __display(0xbfd00420,0x10);
//        //udelay(1000000);
//    __display(NAND_REG_BASE,0x1);  
//    __ww(NAND_REG_BASE+0x0,0x0);
//    __display(NAND_REG_BASE,0x1);

__ww(DMA_DESP,0xa0081100); 
__ww(DMA_DESP+0x4,DDR_PHY); 
__ww(DMA_DESP+0x8,NAND_DEV); 
__ww(DMA_DESP+0xc,0x10);
__ww(DMA_DESP+0x10,0x0); 
__ww(DMA_DESP+0x14,0x1); 
__ww(DMA_DESP+0x18,0x0);
//    //udelay(1000000);
__ww(ORDER_REG_ADDR,DMA_DESP_ORDER);
//    //udelay(1000000);
__display(DMA_DESP,0x10);
__display(ORDER_REG_ADDR,0x1);
//    //udelay(1000000);
__ww(NAND_REG_BASE+0x4,0x800); 
__ww(NAND_REG_BASE+0x1c,0x40); 
//    //udelay(1000000);
__display(NAND_REG_BASE,0X4); 
__ww(NAND_REG_BASE,0x203);
//    //udelay(1000000);
__display(NAND_REG_BASE,0X4); 
__ww(ORDER_REG_ADDR,0x8000004);
__display(0xa8000000,0x8); 
__display(DDR_ADDR,0x10);

//   while(1);
#endif
}

void nanderase(void)
{
	int i=0;
	int status_time ;
	
//	__ww(0xbfd00420,0x0a000000);		//lxy
	__ww(NAND_REG_BASE+0x0, 0x0);   
	__ww(NAND_REG_BASE+0x0, 0x41);	//NAND复位，命令有效
	__ww(NAND_REG_BASE+0x4, 0x00);	//读、写、擦除操作起始地址低32位

	status_time = STATUS_TIME;

	printf("erase blockaddr: 0x%08x", i);
	for(i=0;i<1024;i++){
		printf("\b\b\b\b\b\b\b\b");
		printf("%08x", i<<(11+6));
		__ww(NAND_REG_BASE+0x4, 0x0);   //128K？
		//__ww(NAND_REG_BASE+0x8, i<<(11+6));   //128K？
		__ww(NAND_REG_BASE+0x8, i<<(6));   //128K？
		__ww(NAND_REG_BASE+0x0, 0x8);	//擦除操作
		__ww(NAND_REG_BASE+0x0, 0x9);	//擦除操作 命令有效
		//udelay(2000);
		//外部NAND芯片RDY情况
		//while((*((volatile unsigned int *)(NAND_REG_BASE)) & 0x1<<16) == 0){
		while((*((volatile unsigned int *)(NAND_REG_BASE)) & (0x1<<10)) == 0){
			//udelay(80);
		}
	}
	printf("\nerase all nandflash ok...\n");
}

//#include "nand_gpio.c"

static int nand_read_id(void)
{
	unsigned int cmd_id = 0x21;
	unsigned int cmd_reg = 0xbbee0000;
	unsigned int id_reg =  0xbbee0010;
	unsigned int id_val = 0;
	unsigned int id_val_h = 0;
	int i, dev_id, maf_idx, busw;
	struct mtd_info *mtd = ls1g_soc_mtd;
	struct nand_chip *chip = mtd->priv;
	struct nand_flash_dev *type = NULL;

	*((volatile unsigned int *)(0xbbee000c)) = 0x30f0;	//NAND命令有效需等待的周期数 NAND一次读写所需总时钟周期数
	*((volatile unsigned int *)(0xbbee0000)) = 0x21;		//读ID操作 命令有效
#define    IDL  *((volatile unsigned int*)(0xbbee0010))  
#define    IDH  *((volatile unsigned int*)(0xbbee0014))  
	while(1){
		while(((id_val |= IDL) & 0xff) == 0){
			id_val_h = IDH;
		}
		break;
	}
	/*
	__asm__ __volatile__(
	".set mips3\n"
	"\tlw $4,%1\n"
	"\tlw $5,%2\n"
	"\tsw $4,0($5)\n"
	//            "\tla,a0,%0i"
	"1:\tlw $4,0(%3)\n"
	"\tbeqz $4,1f\n"
	"\tmove %0,$4\n"
	:"=r"(id_val)
	:"r"(cmd_id),"r"(cmd_reg),"r"(id_reg)
	);
	*/
	
	dev_id = (id_val >> 24);
	
	/* Lookup the flash id */
	for (i = 0; nand_flash_ids[i].name != NULL; i++) {
		if (dev_id == nand_flash_ids[i].id) {
			type =  &nand_flash_ids[i];
			break;
		}
	}
	
	if (!type){
		printf("No NAND device found!!!\n");
		return 1;
	}
	
	/* Try to identify manufacturer */
	for (maf_idx = 0; nand_manuf_ids[maf_idx].id != 0x0; maf_idx++) {
		if (nand_manuf_ids[maf_idx].id == (unsigned char)id_val_h)
			break;
	}
	
	busw = type->options & NAND_BUSWIDTH_16;
	
//	printf("read_id_l:0x%08x\nread_id_h:0x%08x\n", id_val, id_val_h);
	printf("NAND device: Manufacturer ID:"
		       " 0x%02x, Chip ID: 0x%02x (%s %s)\n", id_val_h,
		       (id_val >> 24), nand_manuf_ids[maf_idx].name, mtd->name);
	printf("NAND bus width %d instead %d bit\n",
		       (chip->options & NAND_BUSWIDTH_16) ? 16 : 8,
		       busw ? 16 : 8);
	return 0;
}

#define TIMING 0xc
#define OP_NUM  0x1c
#define ADDR    0x4
#define CMD     0x0
static unsigned int nand_num=0;

static void nandwrite_test(int argc,char **argv)/*cmd addr(L,page num) timing op_num(byte) */
{
	unsigned int cmd = 0,addr=0,timing=0x412,op_num=0,dma_num=0;
	unsigned int pages=0,i,val,timeout;
	
	if(argc != 5){
		printf("\nnandwrite_test : cmd  addr(start write page_num)  timing  op_num(write pages)\n");
		printf("EXAMPLE:nandwrite_test : 0x203  0x10 0x412 0x5\n\n");
		return;
	}
	nand_num = 0;
//	__ww(0xbfd00420,0x0a000000);		//lxy
	cmd = strtoul(argv[1],0,0);
	addr = strtoul(argv[2],0,0);
	timing = strtoul(argv[3],0,0);
	op_num = strtoul(argv[4],0,0);
//	dma_num = strtoul(argv[5],0,0);
	if(cmd & 0x9){addr <<= 12;}
	else{addr <<= 11;}
//	pages = (op_num>>11)+1;
	pages = op_num;
	for(i=0;i<pages;i++){
		__ww(NAND_REG_BASE+TIMING, timing);	//0xbbee000c NAND命令有效需等待的周期数 NAND一次读写所需总时钟周期数
		__ww(NAND_REG_BASE+OP_NUM, 0x840);		//0xbbee001c NAND读写操作Byte数；擦除为块数
		__ww(NAND_REG_BASE+ADDR, addr);			//读、写、擦除操作起始地址低32位
		__ww(DDR_ADDR, 0xffffffff);
		/*dma configure*/
		__ww(DMA_DESP, 0x80081100);		//下一个描述符地址寄存器 这里设置为无效
		__ww(DMA_DESP+0x4, DDR_PHY);	//内存地址寄存器
		__ww(DMA_DESP+0x8, NAND_DEV);	//设备地址寄存器
		__ww(DMA_DESP+0xc, 0x210);		//长度寄存器 代表一块被搬运内容的长度，单位是字
		__ww(DMA_DESP+0x10, 0x0);		//间隔长度寄存器 间隔长度说明两块被搬运内存数据块之间的长度，前一个step的结束地址与后一个step的开始地址之间的间隔
		__ww(DMA_DESP+0x14, 0x1);		//循环次数寄存器 循环次数说明在一次DMA操作中需要搬运的块的数目
		__ww(DMA_DESP+0x18, 0x1000);	//控制寄存器 DMA操作类型，“1”为读ddr2写设备，“0”为读设备写ddr2
		__ww(ORDER_REG_ADDR, DMA_DESP_ORDER);	//DMA模块控制寄存器位 在confreg模块，存放第一个DMA描述符地址寄存器
												//#define DMA_DESP_ORDER  0x00800008
												//可以开始读描述符链的第一个DMA描述符
		/*send cmd*/ 
		__ww(NAND_REG_BASE+CMD, cmd&0x200);	//操作发生在NAND的SPARE(备用)区
		__ww(NAND_REG_BASE+CMD, cmd);	
		//udelay(100);
		while(1){
			__ww(ORDER_REG_ADDR,DMA_ASK_ORDER);
			__rw(DMA_DESP+0x4,val);
			if(val == (0x210+DDR_PHY)){break;}
			//udelay(20);
		}
		timeout=30;
		//udelay(300);
		__rw(NAND_REG_BASE+CMD,val);
		while(!(val&0x1<<20)){
			//udelay(20);
			if(!(timeout--)){nand_num++;break;}
			__rw(NAND_REG_BASE+CMD,val);
		}
		printf("nand_num===0x%08x\n", nand_num);
	}
}

//int ls1g_soc_nand_init(void)
void ls1g_soc_nand_init(int argc,char **argv)/*cmd addr(L,page num) timing op_num(byte) */
{
//	nand_gpio_read_id();
//	return 0;
	int val;
	struct nand_chip *this;
	printf("\nNAND DETE\n");
//	nand_test();    
//	if(__rw(0xbfd00420,val) != 0x0a000000)	//lxy
//		__ww(0xbfd00420,0x0a000000);		//lxy


	/* Allocate memory for MTD device structure and private data */
	/* 为MTD设备结构体和nand_chip分配内存 */
	ls1g_soc_mtd = malloc(sizeof(struct mtd_info) + sizeof(struct nand_chip), M_DEVBUF, M_WAITOK);
	if (!ls1g_soc_mtd) {
		printk("Unable to allocate fcr_soc NAND MTD device structure.\n");
		return -ENOMEM;
	}
	/* Get pointer to private data 获得私有数据(nand_chip)指针*/
	this = (struct nand_chip *)(&ls1g_soc_mtd[1]);/*分配内存后的nand_chip结构体指针*/
	/*由于soc_soc_mtd是mtd_info结构体指针 在kmalloc内存分配时加上了+ sizeof(struct nand_chip) 所以&soc_soc_mtd[1]地址相当于指向nand_chip结构体*/

	/* Initialize structures 初始化结构体 清零 */
	memset(ls1g_soc_mtd, 0, sizeof(struct mtd_info));
	memset(this, 0, sizeof(struct nand_chip));

	/* Link(连接) the private(私有的) data with the MTD structure */
	/*将nand_chip赋予mtd_info私有指针*/
	ls1g_soc_mtd->priv = this;
	/* 15 us command delay time 从数据手册获知命令延迟时间 */
	this->chip_delay = 15;
	if(ls1g_nand_init(ls1g_soc_mtd)){
		printf("\n\nerror: PMON nandflash driver have some error!\n\n");
		return -ENXIO;
	}

	/* Scan to find existence of the device 扫描以确定器件的存在*/
	if (nand_scan(ls1g_soc_mtd, 1)) {
		free(ls1g_soc_mtd,M_DEVBUF);	/*扫描失败 soc_soc_mtd可以释放*/
		return -ENXIO;
	}
	if(ls1g_nand_detect(ls1g_soc_mtd)){
		printf("error: PMON driver don't support the NANDFlash!\n ");
		return -ENXIO;
	}

	/* Register the partitions */
//	add_mtd_partitions(fcr_soc_mtd, partition_info, NUM_PARTITIONS);
//
//	add_mtd_device(mtd_info,offset,size,name)
//	add_mtd_device(ls1g_soc_mtd,0,0,"total");

#if 1
	add_mtd_device(ls1g_soc_mtd,0,0x01000000,"kernel");
	add_mtd_device(ls1g_soc_mtd,0x01000000,0x07000000,"os");
//	add_mtd_device(ls1g_soc_mtd,0x07500000,0x00b00000,"data");
#else
	add_mtd_device(ls1g_soc_mtd,0,0x00700000,"kernel");
	add_mtd_device(ls1g_soc_mtd,0x00700000,0x06e00000,"os");
	add_mtd_device(ls1g_soc_mtd,0x07500000,0x00b00000,"data");
#endif

//	find_good_part(ls1g_soc_mtd);
	/* Return happy */
	return 0;
}

static const Cmd Cmds[] =
{
	{"MyCmds"},
	{"nandread","val",0,"nand read addr",nandread,0,99,CMD_REPEAT},
	{"nandwrite","val",0,"nand write addr val",nandwrite,0,99,CMD_REPEAT},
	{"nanderase_verify","val",0,"hardware test",nanderase_verify,0,99,CMD_REPEAT},
	{"nanderase","val",0,"hardware test",nanderase,0,99,CMD_REPEAT},
//	{"nandreadid_gpio","val",0,"hardware test",nand_gpio_read_id,0,99,CMD_REPEAT},
	{"nandreadid","val",0,"hardware test",nand_read_id,0,99,CMD_REPEAT},
	{"nandwrite_test","val",0,"hardware test",nandwrite_test,0,99,CMD_REPEAT},
	{"nandinit","val",0,"hardware test",ls1g_soc_nand_init,0,99,CMD_REPEAT},
	{0, 0}
};

static void init_cmd __P((void)) __attribute__ ((constructor));
static void init_cmd()
{
	cmdlist_expand(Cmds, 1);
}


