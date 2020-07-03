#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


int valid[27] = {0};
typedef struct
{
	int index;
	int row;
	int col;
}parameters;
int sodoku [9][9]={	 
	{6,2,4,5,3,9,1,8,7},
	{5,1,9,7,2,8,6,3,4},
    {8,3,7,6,1,4,2,9,5},
	{1,4,3,8,6,5,7,2,9},
	{9,5,8,2,4,7,3,6,1},
	{7,6,2,3,9,1,4,5,8},
	{3,7,1,9,5,6,8,4,2},
	{4,9,6,1,8,2,5,7,3},
	{2,8,5,4,7,3,9,1,6}};

//判断行是否正确
void *judge_row(void *param)
{
	parameters *params = (parameters*) param;
	int row = params->row;
	int count[10]={0};
	int index = params->index;
	int i =0;
	for (i;i<9;i++)
	{
		if(count[sodoku[row][i]]==1)
			pthread_exit(NULL);
		else
		{
			count[sodoku[row][i]] = 1;
		}
	}
	valid[index] =1;//如果行正确，就把在valid数组中相应行的位置填入1
	printf("Task%d completed\n", index);
	pthread_exit(NULL);
}

//判断列是否正确
void *judge_col(void *param)
{
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->col;	
	int index = params->index;
	int count[10] = {0};
	int i =0;
	for (int i =0;i<9;i++)
	{
		if (count[sodoku[i][col]] == 1) 
			{pthread_exit(NULL);
			printf("exit!\n");}
		else
		{
			count[sodoku[i][col]] = 1;
		}
	}
	valid[index] = 1;//如果列正确，就把在valid数组中相应列的位置填入1
	printf("Task%d completed\n", index);
	pthread_exit(NULL);
}

//判断块是否正确
void *judge_block(void *param)
{
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->col;
	int index = params->index;
	int count[10] = {0};
	int i =row;
	int j = col;
	for(i;i<row+3;i++)
	{
		for (j;j<col+3;j++)
		{
			if (count[sodoku[i][j]] == 1 )
				{pthread_exit(NULL);
				  printf("exit!\n");}
			else
			{
				count[sodoku[i][j]] = 1;
			}
		}
	}
	valid[index] = 1; //如果块正确，就在相应块的位置填入1
	printf("Task%d completed\n", index);
	pthread_exit(NULL);
}

int main(int argc, const char * argv[])
{
	pthread_t tasks[27]; //创建27个线程
	int i = 0;
	int index = 0;
	// 9个线程来判断行是否正确
	for (i;i<9;i++)
	{
		parameters *input = (parameters *) malloc(sizeof(parameters));	
		input->row = i;		
		input->col = 0;
		input->index = index;
		pthread_create(&tasks[index], NULL, judge_row, input);	
		printf("Task%d constructed \n", index);
		index++;
	}
	// 9个线程判断列是否正确
	for (i=0;i<9;i++)
	{
		parameters *input = (parameters *) malloc(sizeof(parameters));	
		input->row = 0;		
		input->col = i;
		input->index = index;
		pthread_create(&tasks[index], NULL, judge_col, input);	
		printf("Task%d constructed \n", index);
		index++;
	}
	// 9个线程判断块是否正确
	int j;
	for (i=0;i<9;i++)
	{
		for (j = 0;j<9;j++)
		{
			if (i%3==0 && j%3 == 0)
			{
				parameters *input = (parameters *) malloc(sizeof(parameters));	
				input->row = i;		
				input->col = j;
				input->index = index;
				pthread_create(&tasks[index], NULL, judge_block, input);	
				printf("Task%d constructed \n", index);
				index++;
			}
		}
	}
	//等待所有线程执行完毕
	for (i = 0; i < 27; i++) 
	{
		pthread_join(tasks[i], NULL);	
	}
	//如果valid数组中有一个为0，说明数组不正确
	for (i=0;i<27;i++)
	{
		if(valid[i] == 0)
			{
			printf("INVALID \n");
			return 0;
			}
	}
	printf("VALID!\n");
	return 0;
}

