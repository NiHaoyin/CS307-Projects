#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 用单链表表示已被占用的内存空间
struct Node{
    int base;
    int limit;
    char pid[20];
    struct Node *next; 
};

// 构造函数
struct Node* create_Node(int base, int limit, char *pid, struct Node* next)
{
    struct Node *p = (struct Node *) malloc(sizeof(struct Node));
    p->base = base;
    p->limit = limit;
    strcpy(p->pid,pid);
    p->next = next;
    return p;
}

int request(char *pid, int memory_size, char method);
void release(char *pid);
void show_status(void);
void compact(void);

static struct Node * head, *rear;

int main(int argc, char * arg[]){
    char command[10];
    char pid[10];
    int required_memory;
	char method;
    if (argc < 2){
        printf("Wrong arguments\n");
        return 1;
    }

    int max_memory = atoi(arg[1]); // 输入最大内存
    rear = create_Node(max_memory,0,"REAR", NULL);
    head = create_Node(0, 0, "HEAD", rear);

    while(1){
        printf("allocator>");
        scanf("%s", command);
        // X=退出
        if (strcmp(command,"X")==0){
            break;
        }
        // RQ=请求内存
        else if(strcmp(command,"RQ")==0){
            scanf("%s", pid);
            scanf("%d", &required_memory);
            while((method = getchar()) ==' ');
            if(request(pid, required_memory, method)){
                printf("Allocation finished\n");
            }
            else{
                printf("Allocation falied\n" );
            }
        }
        // RL=释放内存
        else if (strcmp(command,"RL")==0){
            scanf("%s", pid);
            release(pid);
            printf("%s released\n", pid);
        }
        // STAT=报告当前内存使用情况
        else if (strcmp(command,"STAT")==0){
            show_status();
        }
        // C=压缩孔洞
        else if(strcmp(command,"C")==0){
            compact();
        }
    }
    printf("Exit\n");
    return 0;
}

// First fit
int first_fit(int required_memory, char *pid){
    struct Node *p = head->next;
    struct Node *q = head;
    int space;
    while (p){
        space = p->base-(q->base+q->limit);// space 表示没被占用的内存空间
        if (space < required_memory){
            p=p->next;
            q=q->next;
        }
        else{
            q->next = create_Node(q->base+q->limit, required_memory, pid, p);
            return 1;
        }
    }
    return 0;
}

// Worst fit, 遍历两次链表，第一次找出最大空间，第二次分配这块空间
int worst_fit(int required_memory, char *pid){
    int max_space=-1;
    int space;
    struct Node *p = head->next;
    struct Node *q = head;
    while (p){
        space = p->base-(q->base+q->limit);
        if (space >= required_memory){
            if (space>max_space){
            max_space = space; // 找出最大空间
            }
        }
        p=p->next;
        q=q->next;
    }
    p = head->next;
    q = head;
    while(p){
        space = p->base-(q->base+q->limit);
        if (space == max_space){
            q->next = create_Node(q->base+q->limit, required_memory, pid, p);
            return 1;
            }
        p = p->next;
        q = q->next;
    }
    return 0;
}

// Best fit, 遍历两次链表，第一次找出最小可分配空间，第二次分配这块空间
int best_fit(int required_memory, char *pid){
    int min_diff=10000000;
    int space;
    int diff;
    struct Node *p = head->next;
    struct Node *q = head;
    while(p){
        space = p->base-(q->base+q->limit);
        if (space>= required_memory){
            diff = space-required_memory;
            if(diff<min_diff){
                min_diff=diff;// 找出最小空间可分配空间
            }
        }
    p=p->next;
    q= q->next;
    }
    p = head->next;
    q=head;
    while(p){
        space = p->base-(q->base+q->limit);
        diff = space-required_memory;
        if(diff==min_diff){q->next = create_Node(q->base+q->limit, required_memory, pid, p);
                return 1;
            q->next = create_Node(q->base+q->limit, required_memory, pid, p);
            return 1;
        }
        p=p->next;
        q= q->next;
    }
    return 0;
}

// F=first fit, B=best fit, W=worst fit
int request(char *pid, int required_memory, char method){
    int flag =0;
    if (method=='F'){
        flag = first_fit(required_memory, pid);
    }
    else if (method == 'B'){
        flag = best_fit (required_memory, pid);
    }
    else if (method == 'W'){
        flag=worst_fit(required_memory,pid);
    }
    return flag;
}

// 释放内存空间
void release(char *pid){
    struct Node *p = head->next;
    struct Node *q=head;
    while(p ){
        if(strcmp(p->pid, pid)==0){
            q->next = p->next;
            free(p);
            p = q->next;
        }
        else{
            p=p->next;
            q=q->next;
        }
    }
}

void show_status(void){
    struct Node *p = head->next;
    int start = 0;
    while(p){
        if(p->base != start){
            printf("Address [%d:%d] Unused\n", start, p->base -1);
        }
        if (p->limit != 0){
            start = p->base+p->limit-1;
            printf("Address [%d:%d] Process %s\n", p->base, p->base+p->limit-1, p->pid);
            start++;
        }
        p = p->next;
    }
}

void compact(void){
    struct Node* p = head->next;
    struct Node *q = head;
    while(p != rear){
        p->base = q->base+q->limit;
        p=p->next;
        q=q->next;
    }
}
