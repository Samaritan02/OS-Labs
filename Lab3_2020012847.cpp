#include<stdio.h>
#include<stdlib.h>

#define PROCESS_NAME_LEN 32 	/*进程名长度*/
#define DEFAULT_MEM_SIZE 1024	/*内存大小*/
#define DEFAULT_MEM_START 0		/*起始位置*/

/*描述每一个空闲块的数据结构*/
typedef struct free_block_type{
	int size;
	int start_addr;
	struct free_block_type *next;
}FBT;

/*每个进程分配到的内存块描述*/
typedef struct allocated_block{
	int pid;
	int size;
	int start_addr;
	char process_name[PROCESS_NAME_LEN];
	struct allocated_block *next;
}AB;

/*指向内存中空闲块链表的首指针*/
FBT *free_block;

/*进程分配内存块链表的首指针*/
AB *allocated_block_head = NULL; 

int mem_size = DEFAULT_MEM_SIZE; /*内存大小*/
static int pid = 0; /*初始pid*/
int flag = 0; /*设置内存大小标志*/
int min_mem_size = 16; /*设置剩余分区过小的标志*/

FBT *init_free_block(int mem_size);
void display_menu();
int set_mem_size();
int display_mem_usage();
int dispose(AB *free_ab);
int free_mem();
int kill_process();
int allocate_mem(AB *ab);
int new_process();
void rearrange();


int main(int argc, char const *argv[]){
	/* code */
	char choice;
	pid = 0;
	free_block = init_free_block(mem_size); //初始化空闲区
	while(1){
		fflush(stdin);
		display_menu(); //显示菜单
		fflush(stdin);
		while((choice = getchar()) != '\n'){
		//choice = getchar();
		fflush(stdin);
		switch(choice){
			case '1':set_mem_size();break;
			case '2':rearrange();flag = 1;break;
			case '3':new_process();flag = 1;break;
			case '4':kill_process();flag = 1;break;
			case '5':display_mem_usage();flag = 1;break;
			case '0':exit(0);
			default: break;
		}
		fflush(stdin);
		}
	}
}

void display_menu(){
	puts("");
	printf("1 - Set memory size(fedault=%d)\n",DEFAULT_MEM_SIZE);
	printf("2 - Rearrange memory\n");
	printf("3 - New process\n");
	printf("4 - Terminate a process \n");
	printf("5 - Display memory usage\n");
	printf("0 - Exit\n");
}

//初始化空闲分区链表
FBT *init_free_block(int mem_size){
	FBT *fb;

	fb = (FBT*)malloc(sizeof(FBT));
	if(fb==NULL){
		printf("No mem\n");
		return NULL;
	}
	fb->size = mem_size;
	fb->start_addr = DEFAULT_MEM_START;
	fb->next = NULL;
	return fb;
}

//重新设置内存大小
//将min_mem_size 设为总内存大小的一百分之一 mem_size / 100;
int set_mem_size(){
	int size;
	if(flag!=0){
		printf("Cannot set memory size again\n");
		return 0;
	}
	printf("Total memory size =");
	scanf("%d",&size);
	if(size>0){
		free_block->size = size;
		//设置总内存大小和空闲内存大小
	}
	min_mem_size = size / 100;
	//设置min_mem_size
	flag = 1;
	return 1;
}

int display_mem_usage(){
	//显示当前内存的使用情况，包括空闲分区的情况和已经分配的情况
	FBT *fbt = free_block;
	AB *ab = allocated_block_head;
	printf("\e[0;31;1m------------------------------------------------------------------\e[0m\n");
	//显示空闲区
	printf("\e[0;32;1mFree Memory:\e[0m\n");
	printf("\e[0;33;1m%20s %20s\e[0m\n","     start_addr","       size");
	while(fbt!=NULL){
		printf("%20d %20d\n",fbt->start_addr,fbt->size);
		fbt = fbt->next;
	}

	//显示已分配区
	printf("\n");
	printf("\e[0;35;1mUsed Memory:\e[0m\n");
	printf("\e[0;33;1m%10s %20s %20s %10s\e[0m\n","PID","ProcessName","start_addr","size");
	while(ab != NULL){
		printf("%10d %20s %20d %10d\n",ab->pid,ab->process_name,ab->start_addr,ab->size);
		ab = ab->next;
	}
	printf("\e[0;31;1m------------------------------------------------------------------\e[0m\n");
	return 0;
}

//释放链表节点
int dispose(AB *free_ab){
	/*释放ab数据结构节点*/
	AB *pre,*ab;
	if(free_ab == allocated_block_head){
		//如果要是释放第一个节点
		allocated_block_head = allocated_block_head->next;
		free(free_ab);
		return 1;
	}
	pre = allocated_block_head;
	ab = pre->next;
	while (ab != free_ab)
	{
		pre = pre->next;
		ab = pre->next;
	}
	pre->next = ab->next;
	free(ab);
	return 2;
}

//释放进程所占用的内存
int free_mem(AB *ab){
	/* 将ab所表示的已分配区归还，并进行可能的合并 */
	FBT *fbt,*pre,*work;
	fbt = (FBT*)malloc(sizeof(FBT));
	if(!fbt) return -1;
	/*
	进行可能的合并，基本策略如下
	1. 将新释放的结点插入到空闲分区队列末尾
    2. 对空闲链表按照地址有序排列
    3. 检查并合并相邻的空闲分区
    4. 将空闲链表重新按照当前算法排序
	*/

	fbt->size = ab->size;
	fbt->start_addr = ab->start_addr;

	//插至末尾
	work = free_block;
	if(work == NULL){
		free_block = fbt;
		fbt->next = NULL;
	}else{
		while(work ->next != NULL){
			work = work->next;
		}
		fbt->next = work->next;
		work->next = fbt;
	}

	//按地址重新排布
	rearrange();

	//合并可能分区;即若两空闲分区相连则合并
	pre = free_block;
	while(pre->next){
		work = pre->next;
		if(pre->start_addr + pre->size == work->start_addr ){
			pre->size += work->size;
			pre->next = work->next;
			free(work);
		}else{
			pre = pre->next;
		}
	}

	//按照当前算法排序
	rearrange();
	return 1;
}

//找到pid对应的链表节点
AB *find_process(int pid){
	AB *tmp = allocated_block_head;
	while(tmp != NULL){
		if(tmp->pid==pid)
		{
			return tmp;
		}
		tmp = tmp->next;
	}
	printf("\e[0;31;1m Cannot find pid:%d \e[0m\n",pid);
	return NULL;
}

int kill_process(){
	AB *ab;
	int pid;
	printf("Kill Process,pid=");
	scanf("%d",&pid);
	ab = find_process(pid);
	if(ab!=NULL){
		free_mem(ab);
		dispose(ab);
		//释放ab所表示的分配表
		//释放ab数据结构节点
		return 0;
	}else{
		return -1;
	}
}

//寻找是否有分区可以非进程分配
int find_free_mem(int request){
	FBT *tmp = free_block;
	int mem_sum = 0;
	while(tmp){
		if(tmp->size >= request){
			//可以直接分配
			return 1;
		}
		mem_sum += tmp->size;
		tmp = tmp->next;
		//待补充
	}
	if(mem_sum >= request){
		//合并后分配
		return 0;
	}else{
		//没有足够的空间可供分配
		return -1;
	}

}


//将已分配表按起始地址从大到小排序
void sort_AB(){
	if(allocated_block_head == NULL || allocated_block_head->next == NULL)
		return;
	AB *t1,*t2,*head;
	head = allocated_block_head;
	for(t1 = head->next;t1;t1 = t1->next){
		for(t2 = head;t2 != t1;t2=t2->next){
			if(t2->start_addr > t2->next->start_addr){
				int tmp = t2->pid;
				t2->pid = (t2->next)->pid;
				(t2->next)->pid = tmp;

				tmp = t2->size;
				t2->size = (t2->next)->size;
				(t2->next)->size = tmp;

				tmp = t2->start_addr;
				t2->start_addr = (t2->next)->start_addr;
				(t2->next)->start_addr = tmp;

				char tmpo = (t2->process_name)[0];
				int i = 0;
				while (i < 32)
				{
					(t2->process_name)[i] = ((t2->next)->process_name)[i];
					((t2->next)->process_name)[i] = tmpo;
					i++;
					tmpo = (t2->process_name)[i];
				}
			}
		}
	}
}

//重新给所有进程分配内存地址
void reset_AB(int start){
	/*在真实操作系统中这个操作非常不容易，故内存紧缩并不能频繁使用*/
	AB *tmp = allocated_block_head;
	while(tmp != NULL){
		tmp->start_addr = start;
		start += tmp->size;
		tmp = tmp->next;
	}
}

void  memory_compact(){
	//进行内存紧缩
	FBT *fbttmp = free_block;
	AB *abtmp = allocated_block_head;
	//检测剩余内存
	int sum = 0;
	while(fbttmp!=NULL){
		sum += fbttmp->size;
		fbttmp = fbttmp->next;
	}

	free_block->size = sum;
	free_block->start_addr = 0;
	//合并区块为一个

	//释放多余分区
	FBT *pr = free_block->next;
	while(pr != NULL){
		fbttmp = pr->next;
		free(pr);
		pr = fbttmp;
	}
	free_block->next = NULL;
	//重新排序已分配空间
	sort_AB();
	reset_AB(sum);
}

//执行分配内存
void do_allocate_mem(AB *ab){
	int request = ab->size;
	FBT *tmp = free_block;
	while(tmp != NULL){
		if(tmp->size >= request){
			//分配
			ab->start_addr = tmp->start_addr;
			int restMem = tmp->size - request;
			if(restMem <= min_mem_size){
				printf("%d and %d", restMem, min_mem_size);
				//剩余过小全部分配
				ab->size = tmp->size;
				if(tmp == free_block){
					free_block = free_block->next;
					free(tmp);
				}else{
					FBT *pre = free_block;
					while(pre->next != tmp)
					{
						pre = pre->next;
					}
					pre->next = tmp->next;
					free(tmp);
				}
			}else{
				tmp->size = restMem;
				tmp->start_addr += request;
				//切割出分配走的内存
			}
			return ;
		}
		tmp = tmp->next;
	}
}

int allocate_mem(AB *ab){
	/*分配内存模块*/
	FBT *fbt,*pre;
	int request_size=ab->size;
	fbt = pre = free_block;
	/*
	根据当前算法在空闲分区链表中搜索合适空闲分区进行分配，
		分配时注意以下情况：
    1. 找到可满足空闲分区且分配后剩余空间足够大，则分割
    2. 找到可满足空闲分区且但分配后剩余空间比较小，则一起分配
    3. 找不可满足需要的空闲分区但空闲分区之和能满足需要，
    	则采用内存紧缩技术，进行空闲分区的合并，然后再分配
    4. 在成功分配内存后，应保持空闲分区按照相应算法有序
    5. 分配成功则返回1，否则返回-1
	*/

	//尝试寻找可分配空闲，具体结果在函数中有解释
	int f = find_free_mem(request_size);
	if(f == -1){
		//不够分配
		printf("Free mem is not enough,Allocate fail!\n");
		return -1;
	}else{
		if(f == 0){
			//需要内存紧缩才能分配
			memory_compact();
		}
		//执行分配
		do_allocate_mem(ab);
	}
	//重新排布空闲分区
	rearrange();
	return 1;
} 

//创建新进程
int new_process(){
	AB *ab;
	int size;
	int ret;
	ab = (AB*)malloc(sizeof(AB));
	if(!ab) exit(-5);

	ab->next=NULL;
	pid++;
	sprintf(ab->process_name,"PROCESS-%02d",pid);
	ab->pid = pid;
	printf("Memory for %s:",ab->process_name);
	scanf("%d",&size);
	if(size>0) ab->size=size;
	ret = allocate_mem(ab);		//从空闲分区分配内存，ret==1表示分配成功
	if((ret == 1) && (allocated_block_head == NULL)){
		/*如果此时allocated_block_head尚未赋值，则赋值*/
		allocated_block_head = ab;
		return 1;
	}else if(ret == 1){
		/*分配成功，将该分配块的描述插入已分配链表*/
		ab->next = allocated_block_head;
		allocated_block_head = ab;
		return 2;
	}else if(ret == -1){
		//分配不成功
		printf("\e[0;31;1m Allocation fail \e[0m\n");
		free(ab);
		return -1;
	}
	return 3;
}

void rearrange(){
	/*首次适应算法，空闲区大小按起始地址升序排序*/
	//这里使用冒泡排序方法
	if(free_block == NULL || free_block->next == NULL)
		return;
	FBT *t1,*t2,*head;
	head = free_block;
	for(t1 = head->next;t1;t1 = t1->next){
		for(t2 = head;t2 != t1;t2=t2->next){
			if(t2->start_addr > t2->next->start_addr){
				int tmp = t2->size;
				t2->size = (t2->next)->size;
				(t2->next)->size = tmp;

				tmp = t2->start_addr;
				t2->start_addr = (t2->next)->start_addr;
				(t2->next)->start_addr = tmp;
			}
		}
	}
}