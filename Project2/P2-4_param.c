#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include<linux/types.h>
#include <linux/moduleparam.h>

typedef struct collatz
{
    int num;
    struct list_head list;
}collatz;

static int start = 25;
module_param(start, int, 0);
LIST_HEAD(collatz_list);

int init(void)
{
    //创建链表
    collatz*ele;
    int n = start;
	while(!(n==1))
	{
		ele=kmalloc(sizeof(*ele),GFP_KERNEL);
		ele->num=n;
		INIT_LIST_HEAD(&ele->list);
		list_add_tail(&ele->list,&collatz_list);
		if (n%2==0)
            n = n/2;
        else
            n = n*3+1;
	}
    //遍历链表
    collatz *ptr;
    list_for_each_entry(ptr, &collatz_list,list)
    {
        printk(KERN_INFO "%d  ", ptr->num);
    }
    printk(KERN_INFO "LOADED\n");
    return 0;
}

void m_exit(void)
{
    collatz *ptr, *next;
    list_for_each_entry_safe(ptr,next,&collatz_list,list)
    {
        printk(KERN_INFO "Deleting %d \n", ptr->num);
        list_del(&ptr->list);
        kfree(ptr);
    }
    printk(KERN_INFO "Removed\n");
}

module_init(init);
module_exit(m_exit);


