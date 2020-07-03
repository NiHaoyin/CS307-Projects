#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "task.h"
#include "cpu.h"
#include "schedulers.h"

struct node * head = NULL;
struct node *p, *q;

Task * newTask(char *name, int priority, int burst)
{
    Task *new_task = (Task*)malloc(sizeof(Task));
    new_task->name = (char*) malloc (sizeof(char)*(strlen(name)+1));
    new_task->name = name;
    new_task->burst = burst;
    new_task->priority = priority;
    return new_task;
}

void add(char *name, int priority, int burst) {
	if (head == NULL) {
		head = (struct node *)malloc (sizeof (struct node));
	}
    //p和q是为了实现插入操作
	p = head->next;
    q = head;
    while(p)
    {
        //找到该任务应该插入的位置
        if(priority<=p->task->priority){
            q = q->next;
            p = p->next;
        }
        else{
            break;
        }
    }
    //插入该任务
    q->next = (struct node *) malloc (sizeof (struct node));
    q->next->next = p;
    q->next->task = newTask(name, priority,  burst);
}

void schedule() 
{
	p = head->next;
	while (p) {
		run (p->task, p->task->burst);
		p = p->next;
	}
}
