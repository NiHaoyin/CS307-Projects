#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "task.h"
#include "cpu.h"
#include "schedulers.h"

struct node*head=NULL;
struct node*p;

Task * newTask(char *name, int priority, int burst)
{
    Task *new_task = (Task*)malloc(sizeof(Task));
    new_task->name = (char*) malloc (sizeof(char)*(strlen(name)+1));
    new_task->name = name;
    new_task->burst = burst;
    return new_task;
}

void add(char *name, int priority, int burst) {
	if (head == NULL) {
		head = (struct node *)malloc (sizeof (struct node));
		p = head;
	}
	p->next = (struct node *) malloc (sizeof (struct node));
	p = p->next;
	p->task = newTask;
	p->next = NULL;
}

void schedule()
{
    p=head->next;
    while (p) 
    {
		run (p->task, p->task->burst);
		p= p->next;
	}
}
