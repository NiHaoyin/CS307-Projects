#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"
#include <semaphore.h>
#include <unistd.h>

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3
#define TRUE 1

pthread_mutex_t mutex;

typedef struct 
{
    void (*function)(void *p);
    void *data;
}task;

// 用循环队列来储存任务
task workstodo[QUEUE_SIZE];
int head =0, rear=0;
int flag = 1;

// 工作线程
pthread_t bees[NUMBER_OF_THREADS];

// 入队成功返回0，入队失败返回1
int enqueue(task t) 
{
    if ((rear+1)%QUEUE_SIZE == head){
        return 1;
    }
    workstodo[rear] = t;
    rear=(rear+1)%QUEUE_SIZE;
    return 0;
}

// 出队
task dequeue() 
{
    task worktodo;
    worktodo =workstodo[head];
    head = (head+1)%QUEUE_SIZE;
    return worktodo;
}

// 执行
void *worker(void *param)
{
    task worktodo;
    while(flag){
        if (head == rear){
            sleep(0.5);
        }
        else{
            // 出队时要上锁
            pthread_mutex_lock(&mutex);
            worktodo = dequeue();
            pthread_mutex_unlock(&mutex);
            // 执行
            execute(worktodo.function, worktodo.data);
        }
    }
    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p, int i)
{
    task worktodo;
    worktodo.function = somefunction;
    worktodo.data = p;
    // 入队时要上锁
    pthread_mutex_lock(&mutex);
    if (enqueue(worktodo)){
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

// 初始化线程池，预先创建三个线程
void pool_init(void)
{
    for(int i=0;i<NUMBER_OF_THREADS;i++)
	{
        pthread_create(&bees[i],NULL,worker,NULL);
    }
}
// 关闭线程池
void pool_shutdown(void)
{
    flag = 0;
    for(int i=0;i<NUMBER_OF_THREADS;i++)
	{
	    pthread_join(bees[i],NULL);
	}
}
