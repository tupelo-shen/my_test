核心功能由initmips()函数实现，其代码如下：

    void initmips(unsigned long long msize,unsigned long long dmsize, unsigned long long dctrl)
    {
        long *edata=(void *)0x8f298d40;
        long *end=(void *)0x8f2ddf1c;
        int *p;

        int debug=(msize==0);
         
        //  CPU_TLBClear();
        tgt_puts("Uncompressing Bios");
        
        if(!debug||dctrl&1) enable_cache();
        
        while(1)
        {
            if(run_unzip(biosdata,0x8f010000)>=0)break;
        }

        tgt_puts("OK,Booting Bios\r\n");
        for(p=edata;p<=end;p++)
        {
            *p=0;
        }
        memset(0x8f010000-0x1000,0,0x1000);//0x8f010000-0x1000 for frame(registers),memset for pretty
    #ifdef NOCACHE2
        flush_cache();
    #else
        flush_cache2();
    #endif
        realinitmips(debug?dmsize:msize);
    }