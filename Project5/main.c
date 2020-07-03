#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include "buffer.h"

int main (int argc, char * argv[]) {
	int num_producer, num_consumer, sleeptime;
	printf("Enter sleep time");
	scanf("%d", &sleeptime);
	printf("Enter the number of producers?");
	scanf("%d",&num_producer);
	printf("Enter the number of consumers?");
	scanf("%d",&num_consumer);
	init_buffer();
	int i = 0;
	pthread_t  producers[num_producer], consumers[num_consumer];
	int *c_id = malloc(num_consumer*sizeof(pthread_t));
	int *p_id = malloc(num_producer*sizeof(pthread_t));
	for (i; i < num_producer; i++) {
		p_id[i] = i;
		pthread_create (&producers[i], NULL, producer, &p_id[i]);
	}
	for (i = 0; i < num_consumer; i++) {
		c_id[i] = i;
		pthread_create (&consumers[i], NULL, consumer, &c_id[i]);
	}
	sleep(sleeptime);
	/* 6. Exit */
	for (i = 0; i < num_producer; ++ i) {
		pthread_cancel (producers[i]);
	}
	for (i = 0; i < num_consumer; ++ i) {
		pthread_cancel (consumers[i]);
	}
	for (i = 0; i < num_producer; ++ i) {
		pthread_join (producers[i], NULL);
	}
	for (i = 0; i < num_consumer; ++ i) {
		pthread_join (consumers[i], NULL);
	}
	return 0;
}

