#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

int array[]={7,12,19,3,18,4,2,6,15,8};
int array_length=sizeof(array)/sizeof(array[0]);
typedef struct{
    int low;
    int high;
}parameters;

void merge(int a[],int low,int high)
{
	int mid=(low+high)/2;
	int left=low;
	int right=mid+1;
	int b[high-low+1];
	int p=0;
	while(left<=mid &&right<=high)
	{
		if(a[left]>a[right])
			b[p++]=a[right++];
		else
			b[p++]=a[left++];
	}
	while(left<=mid)
		b[p++]=a[left++];
	while(right<=high)
		b[p++]=a[right++];
    int i;
	for(i=0;i<(high-low+1);i++)
		{
			a[low+i]=b[i];
        }
}

void merge_sort(int arr[],int left,int right)
{
	if(left<right)
	{	
		int middle=(left+right)/2;
		merge_sort(arr,left,middle);
		merge_sort(arr,middle+1,right);
		merge(arr,left,right);
	}
}

void *sort(void *param)
{
	parameters *params = (parameters*) param;
    int low = params->low;
    int high = params->high;
    merge_sort(array,low,high);
}

int main(void)
{
	printf("Original: ");
	for(int i=0;i<array_length;i++)
	{
		printf("%d ",array[i]);
	}
	printf("\n");
	pthread_t tid1;
	pthread_t tid2;
	pthread_t tid3;
    parameters *input1 = (parameters *) malloc(sizeof(parameters));
    input1->low = 0;
    input1->high = 4;
    pthread_create(&tid1, NULL, sort, input1);
    parameters *input2 = (parameters *) malloc(sizeof(parameters));
    input2->low = 5;
    input2->high = 9;
    pthread_create(&tid2, NULL, sort, input2);
    parameters *input3 = (parameters *) malloc(sizeof(parameters));
    input1->low = 0;
    input1->high = 9;
    pthread_create(&tid3, NULL, sort, input3);
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	pthread_join(tid3,NULL);
	printf("Sorted: ");
	for(int i=0;i<array_length;i++)
	{
		printf("%d ",array[i]);
	}
	return 0;
}