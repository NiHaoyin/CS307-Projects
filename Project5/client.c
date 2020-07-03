/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

#define NUM 30

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;
    printf("Add %d and %d equals %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // 创建任务
    struct data works[NUM];
    int i;
    for (i=0;i<NUM;i++){
        works[i].a = i;
        works[i].b =i;
    }
    pool_init();
    // 提交任务
    for (i=0;i<NUM;i++){
        // 如果队列满了，提交失败，就sleep 1秒，然后再次提交
        if(pool_submit(&add,&works[i], i)){
            printf("The queue is full. Please wait a second.\n");
            sleep(1);
            i--;
        }
    }
    sleep(1);
    pool_shutdown();
    return 0;
}
