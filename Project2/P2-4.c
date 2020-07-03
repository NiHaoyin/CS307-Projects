#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include<linux/types.h>

typedef struct color
{
    int num;
    int red;
    int green;
    int blue;
    struct list_head list; // list包括prev和next指针
}color;

static LIST_HEAD(color_list);

int init(void)
{
    //创建链表
    color *ele;
    int j;
    int i ;
    for (i=0;i<10;i++)
    {
        ele = kmalloc(sizeof(*ele), GFP_KERNEL);
        j = 100+i*10;
        ele->num = i;
        ele->red = j;
        ele->blue = j+10;
        ele->green = j+20;
        INIT_LIST_HEAD(&ele->list);
        list_add_tail(&ele->list, &color_list);
    }
    //遍历链表
    color *ptr;
    list_for_each_entry(ptr, &color_list,list)
    {
        printk(KERN_INFO "R=%d  G=%d  B=%d \n", ptr->red,ptr->green,ptr->blue);
    }
    printk(KERN_INFO "LOADED\n");
    return 0;
}

void m_exit(void)
{
    color *ptr, *next;
    // 删除链表
    list_for_each_entry_safe(ptr,next,&color_list,list)
    {
        printk(KERN_INFO "Deleting the %dth color \n", ptr->num);
        list_del(&ptr->list);
        kfree(ptr);
    }
    printk(KERN_INFO "Removed\n");
}

module_init(init);
module_exit(m_exit);


