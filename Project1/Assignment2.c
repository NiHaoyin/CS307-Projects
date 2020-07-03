/*参考教材配套的源码*/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <asm/uaccess.h>
#include<asm/param.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

ssize_t proc_read(struct file *file,char __user *usr_buf,size_t count,loff_t *pos);
long int j1, j2;
static struct file_operations proc_ops ={
.owner = THIS_MODULE,
.read = proc_read,
};

int proc_init(void)
{
    proc_create(PROC_NAME,0666,NULL,&proc_ops);
    printk(KERN_INFO "Created\n");
    j1 = jiffies;
    return 0;
}

void proc_exit(void)
{
    remove_proc_entry(PROC_NAME,NULL);
    printk(KERN_INFO "Removed\n");
}

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv=0;
    char buffer[BUFFER_SIZE];
    // completed使得只输出一次seconds
    static int completed=0;
    if (completed){
      completed=0;
      return 0;
    }
    completed=1;
    j2 = jiffies;
    int seconds = (j2-j1)/HZ;
    rv=sprintf(buffer, "The value of seconds is:%lu\n", seconds);
    raw_copy_to_user(usr_buf,buffer,rv);
    return rv;
}
module_init(proc_init);
module_exit(proc_exit);

