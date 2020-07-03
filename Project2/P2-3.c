#include <linux/init.h>
#include <linux/sched.h>
#include<linux/sched/signal.h>
#include <linux/module.h>
#include <linux/kernel.h>


#define BUFFER_SIZE 128
#define PROC_NAME "pid"

int init(void)
{
    struct task_struct *tsk;
    tsk = pid_task(find_vpid(1), PIDTYPE_PID);
    printk(KERN_INFO "%25s\t%25s\t%25s\n", "command", "pid", "state");
    for_each_process(tsk)
    {
        printk(KERN_INFO "%25s\t%25d\t%25ld\n", tsk->comm, tsk->pid, tsk->state);
    }
    return 0;    
}

static void m_exit(void)
{
    printk(KERN_INFO "Removed");
}


module_init( init );
module_exit( m_exit );
MODULE_AUTHOR("Ni Haoyin");


