#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "task.h"
#include "cpu.h"
#include "schedulers.h"

#define TIME 10

struct node * head = NULL;
struct node * p, *q;

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
	    head->next = NULL;
		head->task = NULL;
        p = head;
    }
	p->next = (struct node *) malloc (sizeof (struct node));
	p = p->next;
	p->task = newTask (name, priority, burst);
	p->next = NULL;
}

void schedule() {
	int remain;
	while(head->next){
        // 一个时间片执行不完
        if (TIME<head->next->task->burst){
            run(head->next->task, TIME);
            //更新remaining time
            remain = head->next->task->burst - TIME;
            head->next->task->burst = remain;
            //将该进程放到链表尾部
            p->next = head->next;
            p = p->next;
            head->next = head->next->next;
            p->next = NULL;
        }
        else {
            //一个时间片能够执行完
            run(head->next->task, head->next->task->burst);
            head->next = head->next->next;
        }
    }
}