#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include "buffer.h"

// 用循环队列实现buffer
buffer_item buffer [BUFFER_SIZE];
int head, rear;
int sleeptime = 2;
sem_t full, empty;
pthread_mutex_t mutex;

int init_buffer (void) {
	head = rear = 0;
	pthread_mutex_init (&mutex, NULL);
	sem_init(&full, 0,0);
	sem_init(&empty, 0, BUFFER_SIZE);
	return 0;
}
// 如果添加失败返回1，添加成功返回0
int insert_item (buffer_item item) {
	sem_wait (&empty);
	pthread_mutex_lock (&mutex);
	if ((rear + 1) % BUFFER_SIZE == head) {
		pthread_mutex_unlock (&mutex);
		return 1;
	} else {
		buffer[rear] = item;
		rear = (rear + 1) % BUFFER_SIZE;
		pthread_mutex_unlock (&mutex);
		sem_post (&full);
		return 0;
	}
}
// 删除成功返回0，删除失败返回1
int remove_item (buffer_item * item) {
	sem_wait (&full);
	pthread_mutex_lock (&mutex);
	if (head == rear) {
		pthread_mutex_unlock (&mutex);
		return 1;
	} else {
		*item = buffer [head];
		head = (head + 1) % BUFFER_SIZE;
		pthread_mutex_unlock (&mutex);
		sem_post (&empty);
		return 0;
	}
}

void *producer(void *param){
	buffer_item  item;
	while(1){
		sleep(rand()%sleeptime);
		item = rand();
		if (insert_item(item)){
			printf("The buffer is full\n");
		}
		else{
			printf("%d inserted by producer %d\n", item, *(int *)param);
		}
	}
}

void * consumer (void * param) {
	buffer_item item;
	while (1) {
		sleep (rand() % sleeptime);
		if (remove_item (&item)) {
			printf("The buffer is empty\n");
		} else {
			printf ("%d consumed by consumer %d\n", item, *(int*)param);
		}
	}
}