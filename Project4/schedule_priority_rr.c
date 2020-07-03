#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "task.h"
#include "cpu.h"
#include "schedulers.h"

#define TIME 10

struct node ** heads = NULL;
struct node ** rears;

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
	int i;
	if (heads == NULL) {
		heads = (struct node **) malloc (sizeof (struct node *)*(MAX_PRIORITY+1));
		rears = (struct node **) malloc (sizeof (struct node *)*(MAX_PRIORITY+1));
		for (i = 0; i <= MAX_PRIORITY; ++ i) {
			heads[i] = rears[i] = NULL;
		}
	}
	if (heads[priority] == NULL) {
		heads[priority] = (struct node *) malloc (sizeof (struct node));
		heads[priority]->task = NULL;
		heads[priority]->next = NULL;
		rears[priority] = heads[priority];
	}
	rears[priority]->next = (struct node *) malloc (sizeof (struct node));
	rears[priority] = rears[priority]->next;
	rears[priority]->task = newTask (name, priority, burst);
	rears[priority]->next = NULL;
}

void schedule_priority(int pri) {
    struct node* head = heads[pri];
    struct node* p = rears[pri];
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

void schedule () {
	int i;
	for (i = MAX_PRIORITY; i >= MIN_PRIORITY; -- i) {
		if (heads[i]) 
			schedule_priority (i);
	}
}

