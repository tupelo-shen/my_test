/*
 * jiq.c -- 模块： just-in-queue
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>               /* everything... */
#include <linux/proc_fs.h>
#include <linux/errno.h>            /* error codes */
#include <linux/workqueue.h>
#include <linux/preempt.h>
#include <linux/interrupt.h>        /* tasklets */

MODULE_LICENSE("Dual BSD/GPL");

/*
 * 延时工作队列定时器文件的延时时间
 */
static long delay = 1;
module_param(delay, long, 0);

/*
 * 本模块所做的工作就是：工作队列中的任务怎样获取系统信息
 */
#define LIMIT       (PAGE_SIZE-128)     /* 打印数据的上限 */

/*
 * 打印当前环境的信息。由任务队列调用。如果达到限制，则唤醒读取进程。
 */
static DECLARE_WAIT_QUEUE_HEAD      (jiq_wait);

/*
 * 声明tasklet使用的数据结构
 */
//static struct work_struct jiq_work; 

/*
 * 跟踪我们所需的任务队列运行时的信息。
 */
struct client_data
{
    int             len;
    char            *buf;
    unsigned long   jiffies;
    long            delay;
};

static struct MY_WORK
{
    struct work_struct  jiq_work;
    struct client_data  jiq_data;
} my_work;

//#define SCHEDULER_QUEUE     ((task_queue *) 1)

static void jiq_print_tasklet(unsigned long);
static DECLARE_TASKLET(jiq_tasklet, jiq_print_tasklet, (unsigned long)&(my_work.jiq_data));

/*
 * 打印信息； 如果任务需要重新调度，则返回非0
 */
static int jiq_print(void *ptr)
{
    struct client_data  *data = (struct client_data *)ptr;
    int                 len = data->len;
    char                *buf = data->buf;
    unsigned long       j = jiffies;

    if(len > LIMIT)
    {
        wake_up_interruptible(&jiq_wait);
        return 0;
    }

    if(len == 0)
    {
        len = sprintf(buf, "    time  delta preempt   pid cpu command\n");
    }
    else
        len = 0;

    /* intr_count is only exported since 1.3.5, but 1.99.4 is needed anyways */
    len += sprintf(buf+len, "%9li  %4li     %3i %5i %3i %s\n",
            j, j - data->jiffies,
            preempt_count(), current->pid, smp_processor_id(),
            current->comm);

    data->len += len;
    data->buf += len;
    data->jiffies = j;
    return 1;
}

/*
 * 从工作队列中调用jiq_print
 */
#if 0
static void jiq_print_wq(void *ptr)
{
    struct client_data *data = (struct client_data *)ptr;
    
    if (!jiq_print(ptr))
        return;
    
    if (data->delay)
        schedule_delayed_work(&jiq_work, data->delay);
    else
        schedule_work(&jiq_work);
}
#else
static void jiq_print_wq(struct work_struct* work)
{
    struct client_data *data = (struct client_data *)&(container_of(work, 
                                struct MY_WORK, jiq_work)->jiq_data);
    
    if (!jiq_print((void*)data))
        return;
    
    if (data->delay)
        //schedule_delayed_work(&jiq_work, data->delay);
        schedule_delayed_work(work, data->delay);
    else
        //schedule_work(&jiq_work);
        schedule_work(work);
}
#endif

/*
 * 与/proc文件绑定的函数
 */
static int jiq_read_wq(char *buf, char **start, off_t offset, int len, 
    int *eof, void *data)
{
    DEFINE_WAIT(wait);
    
    (my_work.jiq_data).len = 0;                /* 初始化，没有数据打印 */
    (my_work.jiq_data).buf = buf;              /* 打印数据的位置 */
    (my_work.jiq_data).jiffies = jiffies;      /* 开始时间 */
    (my_work.jiq_data).delay = 0;
    
    prepare_to_wait(&jiq_wait, &wait, TASK_INTERRUPTIBLE);
    schedule_work(&(my_work.jiq_work));
    schedule();
    finish_wait(&jiq_wait, &wait);

    *eof = 1;
    return (my_work.jiq_data).len;
}

/*
 * 与/proc文件绑定的函数（带有延时处理）
 */
static int jiq_read_wq_delayed(char *buf, char **start, off_t offset, 
    int len, int *eof, void *data)
{
    DEFINE_WAIT(wait);
    
    (my_work.jiq_data).len = 0;                /* 初始化，没有数据打印 */
    (my_work.jiq_data).buf = buf;              /* 打印数据的位置 */
    (my_work.jiq_data).jiffies = jiffies;      /* 开始时间 */
    (my_work.jiq_data).delay = delay;
    
    prepare_to_wait(&jiq_wait, &wait, TASK_INTERRUPTIBLE);
    schedule_delayed_work(&(my_work.jiq_work), delay);
    schedule();
    finish_wait(&jiq_wait, &wait);

    *eof = 1;
    return (my_work.jiq_data).len;
}

/*
 * 从tasklet中调用jiq_print
 */
static void jiq_print_tasklet(unsigned long ptr)
{
    if (jiq_print((void *)ptr))
        tasklet_schedule (&jiq_tasklet);
}


/*
 * 与/proc文件绑定的函数
 */
static int jiq_read_tasklet(char *buf, char **start, off_t offset, int len, 
    int *eof, void *data)
{
    (my_work.jiq_data).len = 0;                /* 初始化，没有数据打印 */
    (my_work.jiq_data).buf = buf;              /* 打印数据的位置 */
    (my_work.jiq_data).jiffies = jiffies;      /* 开始时间 */

    tasklet_schedule(&jiq_tasklet);
    interruptible_sleep_on(&jiq_wait);    /* 休眠直到完成 */

    *eof = 1;
    return (my_work.jiq_data).len;
}




/*
 * 测试定时器。
 */

static struct timer_list jiq_timer;     /* 声明一个定时器 */

static void jiq_timedout(unsigned long ptr)
{
    jiq_print((void *)ptr);             /* 打印一行 */
    wake_up_interruptible(&jiq_wait);   /* 唤醒进程 */
}


static int jiq_read_run_timer(char *buf, char **start, off_t offset, 
    int len, int *eof, void *data)
{

    (my_work.jiq_data).len = 0;                   /* 为函数jiq_print()准备参数 */
    (my_work.jiq_data).buf = buf;
    (my_work.jiq_data).jiffies = jiffies;

    init_timer(&jiq_timer);             /* 初始化定时器数据结构 */
    jiq_timer.function = jiq_timedout;
    jiq_timer.data = (unsigned long)&(my_work.jiq_data);
    jiq_timer.expires = jiffies + HZ;   /* 1秒 */

    jiq_print(&(my_work.jiq_data));               /* 打印，然后休眠 */
    add_timer(&jiq_timer);
    interruptible_sleep_on(&jiq_wait);  /* 竞态 */
    del_timer_sync(&jiq_timer);         /* 以防信号唤醒 */
    
    *eof = 1;
    return (my_work.jiq_data).len;
}

/*
 * 模块init函数
 */

static int jiq_init(void)
{

    /* 初始化work， 添加到工作队列 */
    //INIT_WORK(&jiq_work, jiq_print_wq, &jiq_data);
    INIT_WORK(&(my_work.jiq_work), jiq_print_wq);

    create_proc_read_entry("jiqwq", 0, NULL, jiq_read_wq, NULL);
    create_proc_read_entry("jiqwqdelay", 0, NULL, jiq_read_wq_delayed, NULL);
    create_proc_read_entry("jitimer", 0, NULL, jiq_read_run_timer, NULL);
    create_proc_read_entry("jiqtasklet", 0, NULL, jiq_read_tasklet, NULL);

    return 0; /* succeed */
}

static void jiq_cleanup(void)
{
    remove_proc_entry("jiqwq", NULL);
    remove_proc_entry("jiqwqdelay", NULL);
    remove_proc_entry("jitimer", NULL);
    remove_proc_entry("jiqtasklet", NULL);
}


module_init(jiq_init);
module_exit(jiq_cleanup);