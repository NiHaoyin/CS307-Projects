#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define PAGE_TABLE_ENTRIES_NUM 256
#define PAGE_SIZE 256
#define TLB_ENTRIES_NUM 16
#define FRAME_SIZE 256
#define NUM_FRAMES 256
#define MEMORY_SIZE FRAME_SIZE*NUM_FRAMES

typedef struct ffl_entry {
    int frame_num;
    struct ffl_entry *next;
}free_frame_list_entry;

free_frame_list_entry *free_frame_list;
char *memory;

typedef struct page_table_stack_node {
    int page_num;
    struct page_table_stack_node *prev;
    struct page_table_stack_node *next;
}page_node;

page_node *page_top;
page_node *page_bottom;

typedef struct {
    int frame_num;
    int valid;
    page_node *node;
}page_table_entry;

typedef struct {
    int page_num;
    int frame_num;
}TLB_entry;

page_table_entry *page_table;
TLB_entry *TLB;
int free_TLB_index;
int *TLB_cnts;

int num_references;
int num_page_faults;
int num_TLB_hits;
int num_correct_reads;

/* extract page number and offset from an address */
void extract_address (int address, int * page_number, int * offset);

/* translate from logical address to physical address */
int translate (int logical_add, FILE * f_backing_store, int time_cnt);

/* read a byte from memory using physical address */
char read_from_memory (int physical_add);

// initialize the manager
void manager_init (void);

/* clear page table and TLB */
void page_table_clear (void);

/* initialize memory and free frame list */


/* clear physical memory and free frame list */
void memory_clear (void);

/* get a free frame in physical memory*/
int get_free_frame (void);

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Wrong parameters\n");
        return 1;
    }
    int logical_add, physical_add;
    FILE * addresses, *backing_store, *correct;
    manager_init();
    backing_store = fopen ("BACKING_STORE.bin", "rb");
    correct = fopen ("correct.txt", "r");
    if (backing_store == NULL){
        printf("Fail to open BACKING_STORE.bin\n");
        return 1;
    }
    if (correct == NULL){
        printf("Fail to open correct.txt\n");
        return 1;
    }
    addresses = fopen(argv[1], "r");
    if (addresses = NULL){
        printf("Fail to open %s\n", argv[1]);
        return 1;
    }
    int i = 0; 
    char content;
    int correct_logical_add, correct_physical_add, correct_value;
	while (~fscanf (addresses, "%d", &logical_add)) {
		physical_add = translate (logical_add, backing_store, i);
		content = read_from_memory (physical_add);
		printf ("Logical address: %d Physical address: %d Value: %d\n", logical_add, physical_add, content);
		i++;

		// check read result
		fscanf (correct, "Logical address: %d Physical address: %d Value: %d\n", &correct_logical_add, &correct_physical_add, &correct_value);
		if (correct_logical_add != logical_add) {
			printf ("[warning] inconsistent correct answer, i=%d, logical_add=%d, correct_logical_add=%d\n", i, logical_add, correct_logical_add);
		}
		if (correct_value == (int) content)
			num_correct_reads++;
	}
    printf ("Statistics are shown as following:\n");
	printf ("Page fault rate: %f\n", (double) num_page_faults / num_references);
	printf ("TLB hit rate: %f\n", (double) num_TLB_hits / num_references);
    fclose (addresses);
	fclose (backing_store);
	fclose (correct);
    return 0;
}

int get_free_frame(void){
    free_frame_list_entry *tmp;
    int frame_num;
    page_node *ptr;
    if (free_frame_list == NULL){
        ptr = page_bottom->prev;
        if (ptr == page_top){
            printf("Page table is empty\n");
            return 0;
        }
        page_table[ptr->page_num].valid = 0;
        page_bottom->prev = ptr->prev;
        ptr->prev->next = page_bottom;
        free_frame_list = (free_frame_list_entry *) malloc (sizeof (free_frame_list_entry));
        free_frame_list->frame_num = page_table[ptr->page_num].frame_num;
        free_frame_list->next = NULL;
    }
    tmp = free_frame_list;
    free_frame_list = tmp->next;
    frame_num = tmp->frame_num;
    free(tmp);
    return frame_num;
}

void extract_address (int address, int * page_number, int * offset) {
	int mask = 0xff;
	*offset = address & mask;
	*page_number = (address >> 8) & mask;
}

int translate(int logical_add, FILE *backing_store, int time_cnt){
    int page_num, offset;
    int frame_num;
    int physical_add;
    int TLB_index;
    int tmp1, tmp2;
    page_node *page_ptr;

    num_references++;
    extract_address(logical_add, &page_num, &offset);
    int i = 0;
    for (i;i<free_TLB_index;i++){
        if(page_num == TLB[i].page_num){
            num_TLB_hits++;
            TLB_index = i;
            break;
        }
    }
    if (i == free_TLB_index){
        if (page_table[page_num].valid == 0){
            num_page_faults++;
            frame_num = get_free_frame();
            fseek(backing_store,page_num*PAGE_SIZE, SEEK_SET);
            fread(memory+frame_num*FRAME_SIZE,sizeof(char),FRAME_SIZE,backing_store);
            page_table[page_num].valid = 1;
            page_table[page_num].frame_num = frame_num;
            page_table[page_num].node->next = page_top->next;
            page_top->next->prev = page_table[page_num].node;
            page_top->next = page_table[page_num].node;
            page_table[page_num].node->prev = page_top;
        }
        frame_num = page_table[page_num].frame_num;
        if (free_TLB_index <TLB_ENTRIES_NUM){
            TLB_index = free_TLB_index;
            free_TLB_index++;
        }
        else{
            tmp1 = TLB_cnts[0];
            tmp2 = 0;
            for (int i=0;i<TLB_ENTRIES_NUM;i++){
                if (TLB_cnts[i]<tmp1){
                    tmp1 = TLB_cnts[i];
                    tmp2 = i;
                }
            }
            TLB_index = tmp2;
        }
        TLB[TLB_index].page_num = page_num;
        TLB[TLB_index].frame_num = frame_num; 
    }
    page_ptr = page_table[page_num].node->next;
    page_table[page_num].node->next = page_top->next;
    page_top->next->prev = page_table[page_num].node;
    page_top->next = page_table[page_num].node;
    page_ptr->prev = page_table[page_num].node->prev;
    page_table[page_num].node->prev->next = page_ptr;
    page_table[page_num].node->prev = page_top;

	frame_num = TLB[TLB_index].frame_num;
	TLB_cnts[TLB_index] = time_cnt; // set the counter
	physical_add = (frame_num << 8) | offset;
	fseek (backing_store, 0, SEEK_SET);
	return physical_add;
}

char read_from_memory(int physical_add){
    return memory[physical_add];
}
    
void manager_init(void){
    // initialize pagetable
    page_table = (page_table_entry*)malloc(sizeof (page_table_entry) * PAGE_TABLE_ENTRIES_NUM);
    TLB = (TLB_entry*)malloc (sizeof (TLB_entry) * TLB_ENTRIES_NUM);
    free_TLB_index = 0;
    TLB_cnts = (int*)malloc (sizeof (int) * TLB_ENTRIES_NUM);
    page_top = (page_node *) malloc (sizeof(page_node));
	page_bottom = (page_node *) malloc (sizeof (page_node));
    page_top->prev = NULL;
    page_top->next = NULL;
    page_bottom->next = NULL;
    page_bottom->prev = NULL;

    int i;
    for (i=0;i<PAGE_TABLE_ENTRIES_NUM;i++){
        page_table[i].valid = 0;
        page_table[i].node=(page_node*)malloc(sizeof(page_node));
        page_table[i].node->page_num = i;
    }
    free_frame_list_entry *tmp;
    memory=(char*)malloc(MEMORY_SIZE*sizeof(char));
    free_frame_list = NULL;
    int num_frame = MEMORY_SIZE/FRAME_SIZE;
    for (i=0;i<num_frame;i++){
        tmp = free_frame_list;
        free_frame_list = (free_frame_list_entry *) malloc (sizeof (free_frame_list_entry));
		free_frame_list->frame_num = num_frame - i - 1;
		free_frame_list->next = tmp;
    }

    num_references = 0;
	num_page_faults = 0;
	num_TLB_hits = 0;
	num_correct_reads = 0;
}

