#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "pid"

static int int_pid;

static ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
    .write = proc_write,
};


static int proc_init(void) {
    // creates the /proc/procfs entry
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}


static void proc_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    struct task_struct *tsk = NULL;
    if (completed) {
        completed = 0;
        return 0;
    }
    tsk = pid_task(find_vpid(int_pid), PIDTYPE_PID);
    if(tsk) 
    {
        rv = snprintf(buffer, BUFFER_SIZE,
                      "command = [%s], pid = [%d], state = [%ld]\n",
                      tsk->comm, int_pid, tsk->state);
    } 
    else 
    {
        printk(KERN_INFO "Invalid PID ");
        return 0;
    }
    completed = 1;
    if (raw_copy_to_user(usr_buf, buffer, rv)) {
        rv = -1;
    }
    return rv;
}

static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    char *k_mem;
    // 分配一块内存给pid(字符串)
    k_mem = kmalloc(count, GFP_KERNEL);
    if (raw_copy_from_user(k_mem, usr_buf, count)) 
    {
        printk( KERN_INFO "Error ");
        return -1;
    }
    // 把pid从字符串变成int
    k_mem[count] = '\0';   // 如果想用kstrtoint，就必须保证字符串结尾是NULL
    kstrtoint(k_mem, 10, &int_pid);
    kfree(k_mem);
    return count;
}

module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");  //LICENSE必不可少，并且得用GPL

MODULE_AUTHOR("Ni Haoyin");


