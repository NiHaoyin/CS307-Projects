#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define NUM_OF_CUSTOMERS 5
#define NUM_OF_RESOURCES 4
#define FILE_NAME "resources.txt"

int available[NUM_OF_RESOURCES];
int maximum[NUM_OF_CUSTOMERS][NUM_OF_RESOURCES];
int allocation[NUM_OF_CUSTOMERS][NUM_OF_RESOURCES];
int need[NUM_OF_CUSTOMERS][NUM_OF_RESOURCES];

//输出全局变量
void output(void){
    int i=0,j=0;
    printf("Available:\n");
    for (i;i<NUM_OF_RESOURCES;i++){
        printf("%d ", available[i]);
    }
    printf("\n");
    printf("Allocation:\n");
    for (i=0;i<NUM_OF_CUSTOMERS;i++){
        for (j=0;j<NUM_OF_RESOURCES;j++){
            printf("%d ",allocation[i][j]);
        }
        printf("\n");
    }
    printf("Need:\n");
    for(i=0;i<NUM_OF_CUSTOMERS;i++){
        for (j=0;j<NUM_OF_RESOURCES;j++){
            printf("%d ", need[i][j]);
        }
    printf("\n");
    }
    printf("Maximum:\n");
    for(i=0;i<NUM_OF_CUSTOMERS;i++){
        for (j=0;j<NUM_OF_RESOURCES;j++){
            printf("%d ",maximum[i][j]);
        }
        printf("\n");
    }
}

//能够执行返回0，不能执行返回1
int satisfy(int need[], int available[]){
    int i =0;
    for (i;i<NUM_OF_RESOURCES;i++){
        if(need[i]>available[i]){
            return 1;
        }
    }
    return 0;
}

//安全返回0，不安全返回1
int check(void){
    //复制全局变量
    int simu_allocation[NUM_OF_CUSTOMERS][NUM_OF_RESOURCES]={0,0};
    int simu_need[NUM_OF_CUSTOMERS][NUM_OF_RESOURCES];
    int simu_available[NUM_OF_RESOURCES];
    memcpy(simu_allocation, allocation, sizeof(allocation));
    memcpy(simu_need, allocation, sizeof(need));
    memcpy(simu_available, allocation, sizeof(available));

    //state为0代表未完成，为1代表已完成
    int state[NUM_OF_CUSTOMERS];
    int i = 0;
    for (i;i<NUM_OF_CUSTOMERS;i++){
        state[i] = 0;
    }

    //开始模拟执行
    for (i=0;i<NUM_OF_CUSTOMERS;i++){
        if (state[i]==0 && satisfy(simu_need[i], simu_available)==0){
            state[i]=1;
            for(int j=0;j<NUM_OF_RESOURCES;j++){
                simu_available[j] = simu_available[j]+simu_allocation[i][j];
            }
            i = -1;
        }
    }
    
    //检查是否所有进程都完成了,如果有进程没完成，说明处于危险状态，返回1
    for(i=0;i<NUM_OF_CUSTOMERS;i++){
        if (state[i] == 0){
            return 1;
        }
    }
    return 0;
}

//能够申请返回0，不能申请返回1
int request_resources(int customer_num, int request[]){
    if (satisfy(request, available) ){
        printf("Request denied because there is no enough available resource\n");
        return 1;
    }

    //复制当前状态，方便回滚
    int prev_allocation[NUM_OF_CUSTOMERS][NUM_OF_RESOURCES];
    int prev_need[NUM_OF_CUSTOMERS][NUM_OF_RESOURCES];
    int prev_available[NUM_OF_RESOURCES];
    memcpy(prev_allocation, allocation, sizeof(allocation));
    memcpy(prev_need, need, sizeof(need));
    memcpy(prev_available, available, sizeof(available));

    //分配资源
    int i=0;
    for (i;i<NUM_OF_RESOURCES;i++){
        available[i]=available[i]-request[i];
        need[customer_num][i] = need[customer_num][i]-request[i];
        allocation[customer_num][i]=allocation[customer_num][i]+request[i];
    }

    //如果申请之后处于不安全状态，就回滚
    if (check()){
        printf("Unsafe");
        memcpy(allocation, prev_allocation, sizeof(allocation));
        memcpy(need, prev_need, sizeof(need));
        memcpy(available, prev_available, sizeof(available));
        return 1;//不安全状态返回1
    }
    return 0;//安全状态返回0
}

//释放进程
void request_release(int customer_num, int request[]){
    int i;
    for (i=0;i<NUM_OF_RESOURCES;i++){
        allocation[customer_num][i]=allocation[customer_num][i]-request[i];
        available[i]= available[i]+request[i];
    }
}

//初始化
int init(int argc, char*argv[], const char*resources_file){
    if (argc != 1+NUM_OF_RESOURCES){
        printf("Parameters incorrect\n");
        return -1;
    }
    for (int i=0;i != NUM_OF_RESOURCES;i++){
        available[i] = atoi(argv[i+1]);
    }
    FILE *f = fopen(resources_file, "r");
    if (f==NULL){
        printf("Fail to open file\n");
        return -2;
    }
    for (int i=0;i<NUM_OF_CUSTOMERS;i++){
        
        for(int j=0;j<NUM_OF_RESOURCES;j++){
            fscanf(f, "%d", &maximum[i][j]);
            need[i][j]=maximum[i][j];
            allocation[i][j]=0;
        }
    }
    fclose(f);
    printf("Initialization finished\n");
    return 0;
}

int main(int argc, char*argv[]){
    if (init(argc,argv,FILE_NAME)){
        printf("Initialization failed\n");
        return 0;
    }
    char operation[10];
    printf(">");
    while (scanf("%s", operation) == 1){
        if (strcmp(operation, "RQ") == 0){
            int request[NUM_OF_RESOURCES];
            int customer_num;
            scanf("%d", &customer_num);
            for (int i=0;i<NUM_OF_RESOURCES;i++){
                scanf("%d", &request[i]);
            } 
            if (request_resources(customer_num, request) == 1){
                printf("Request denied\n");
            }else{
                printf("Request accepted\n");
            }
        }
        else if(strcmp(operation,"RL") == 0){
            int request[NUM_OF_RESOURCES];
            int customer_num;
            scanf("%d", &customer_num);
            for (int i=0;i<NUM_OF_RESOURCES;i++){
                scanf("%d", &request[i]);
            } 
            request_release(customer_num, request);
            printf("%d released\n", customer_num);
        }
        else if (strcmp(operation,"*") == 0){
            output();
        }
        printf("<");
    }
    return 0;
}
