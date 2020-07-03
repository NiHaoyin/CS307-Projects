#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/sched/signal.h>

void dfs(struct task_struct *tsk)
{
	struct task_struct *tsk_next;
	struct list_head *list;
	list_for_each(list,&tsk->children)
	{
		tsk_next=list_entry(list,struct task_struct,sibling);
		printk(KERN_INFO "%25s\t%25d\t%25ld\n",tsk_next->comm,tsk_next->pid,tsk_next->state);
		dfs(tsk_next);
	}
}

int init(void)
{
	printk(KERN_INFO "%25s\t%25s\t%25s\n", "command", "pid", "state");
	dfs(&init_task);
	return 0;
}

void m_exit(void)
{
	printk(KERN_INFO "Removed.\n");
}

module_init(init);
module_exit(m_exit);


