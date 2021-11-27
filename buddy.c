
#include "buddy.h"
#include <sys/mman.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

//Define your Constants here
# define MIN 5
# define LEVELS  8
# define PAGE  4096


enum AEflag { Free, Taken };

struct head {
	enum AEflag status;
	short int level;
	struct head* next;
	struct head* prev;
};

struct head*flists [LEVELS] = {NULL};


//Complete the implementation of new here
struct head *new() {
	
	struct head*block = (struct head *)mmap ( NULL, PAGE,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );

	if(block == MAP_FAILED){
		return NULL;
	}

	else{

		block->status = Free;
		block->level = 7;
		
		return block;

	}

}

//Complete the implementation of level here
int level(int req) {
	//need extra space to hide the header
	int total_size = req + sizeof(struct head);
	int lvl = 0;
	int size = 32;
	while(total_size > size){
		size *= 2 ;
		lvl++;
	}
	return lvl;	
}






//Helper Functions
struct head* buddy(struct head* block) {
	int index = block->level;
	long int mask = 0x1 << (index + MIN);
	return (struct head*)((long int)block ^ mask);

}

struct head* split(struct head* block) {
	int index = block->level - 1;
	int mask = 0x1 << (index + MIN);
	return (struct head*)((long int)block | mask);
}


struct head* primary(struct head* block) {
	int index = block->level;
	long int mask = 0xffffffffffffffff << (1 + index + MIN);
	return (struct head*)((long int)block & mask);
}

void* hide(struct head* block) {
	return (void*)(block + 1);
}


struct head* magic(void* memory) {
	return ((struct head*)memory - 1);
}
	


void dispblocklevel(struct head* block){
	printf("block level = %d\n",block->level);
}
void dispblockstatus(struct head* block){
	printf("block status = %d\n",block->status);
}

void blockinfo(struct head* block){
	printf("===================================================================\n");
	dispblockstatus(block);
	dispblocklevel(block);
	printf("start of block in memory: %p\n", block);
	printf("size of block in memory: %ld in bytes\n",sizeof(struct head));
	printf("===================================================================\n");
}

//prints the free list at each level
void printflists(){

	for(int i = 0 ; i < 8 ; i++){
		printf("========================LEVEL %d====================================\n",i);
		if(flists[i] != NULL){

			
			struct head*temp = flists[i];

			blockinfo(temp);
			while(temp->prev != NULL){
				temp = temp->prev;
				blockinfo(temp);
			}
		
		}
		else{
			printf("NULL\n");
			printf("===================================================================\n");
		}
			
	}
	
	
}

//gets a block at a certain level in the free lists
void* getnode(int n){
	return flists[n];
}

void* FindFree(int lvl){
	//check if a free block is in the requested block level
	if(flists[lvl] != NULL){
		
		//the top of the flists has a free block return it
		if(flists[lvl]->status == Free){

			flists[lvl]->status = Taken;
			
			return flists[lvl];
		}
		else{
			//searching the linked list at that req level for a free block
			struct head*temp = flists[lvl];
			while(temp->prev != NULL){
				temp = temp->prev;
				//if that block is free  update status hide and return
				if(temp->status == Free){
					temp->status = Taken;	
					return temp;
				}
			}
		}
	}

	return NULL;


}

void* FindNextFree(int lvl){
	//find the next free block level in the flists
	for(int i = lvl+1 ; i < 8 ; i ++){
	
		if(flists[i] != NULL){
			if(flists[i]->status == Free){
				
				return flists[i];
				
			}
			else{
				struct head*temp = flists[i];

				while(temp->prev != NULL){

					temp = temp->prev;
					//if that block is free assign it
					if(temp->status == Free){
						
						//make this free block the head of the list
						//unlinking it
						if(temp->prev == NULL){
							if(temp->next != NULL){
								temp->next->prev = NULL;
							}
							
						}
						else{
							if(temp->next != NULL){
								temp->next->prev = temp->prev;
								temp->prev->next = temp->next;
							}
						}
						flists[i]->next = temp;
						temp->prev = flists[i];
						temp->next = NULL;
						flists[i] = temp;
						return temp;
					}
				}
			}
		}
		
	}
	return NULL;
}

void* balloc(size_t size) {
	
	if(size == 0){
		return NULL;
	}
	//get the required level
	
	int lvl = level(size);
	if(lvl > 7){
		printf("Requested Size Too Large\n");
		return NULL;
	}

	//keeping track of which level we are on 
	int Ulvl = 7;
	//the block of memory to be returned
	struct head*unallocated = NULL;

	//finding a freeblock
	unallocated = FindFree(lvl);
	//if there is no free block at that req level search next level
	if(unallocated == NULL){
		unallocated = FindNextFree(lvl);
	}
	else{
		return hide(unallocated);
	}

	
	//if the above FindFree and FindNextFree fails we dont have any free blocks
	//need to allocate one!
	if(unallocated == NULL){
		unallocated = new();
		//inserting this new block into the flists
		if(flists[Ulvl] != NULL){
			flists[Ulvl]->next = unallocated;
			unallocated->prev = flists[Ulvl];
			flists[Ulvl] = unallocated;
		}
		else{
			flists[Ulvl] = unallocated;
		}
	}
	else{
		Ulvl = unallocated->level;
	}

	///////////////SPLITING//////////////
	//split from the highest unallocated block levl i.e Ulvl to the requested level i.e lvl
	for(int i = Ulvl ; i > lvl ; i--){
		//get the unallocated block
		//NOTE code structured such that unallocated will allways be the head
		
		unallocated = flists[i];
	
		//check if unallocated(free) block is connected to anything
		//NOTE code structured such that ->prev will be the next block 
		if(unallocated->prev != NULL){
			//make the next block the head of the list
			flists[i] = unallocated->prev;
			//update its next pointer to null (removes unallocated)
			flists[i]->next = NULL;
		}	
		else{
			//unallocated(free)block is the only block in that level in flists
			flists[i] = NULL;
		}

		//split unallocated(Free) block
		struct head*temp_split = split(unallocated);

		temp_split->prev = unallocated;
		unallocated->next = temp_split;

		temp_split->level = (i-1);
		unallocated->level = (i-1);
		//place the blocks one level up the list
		if(flists[i-1] != NULL){
			flists[i-1]->next = unallocated;
			unallocated->prev = flists[i-1];
			flists[i-1] = temp_split;
			
		}	
		else{
			unallocated->prev = NULL;
			flists[i-1] = temp_split;
		}

	}
	
	// hide and return after spliting the blocks
	unallocated->status = Taken;
	return hide(unallocated);

}

void Unlink(struct head*blocksPrimary , int lvl){
	//unlink the block and buddy from its list
	//i.e unlink the primary block 
	//NOTE code struct such that the primary will always be left of the buddy
	//primary is at the far left
		if(blocksPrimary->prev == NULL){

			//if the primary is the only block in the level of flists
			//we can set that level to NULL
			if(blocksPrimary->next->next == NULL){
			
				flists[lvl] = NULL;
			}
			else{
				//update the ->next->next->prev to point to null to unlink the buddy 
				blocksPrimary->next->next->prev = NULL;	
			}
		
		}
		else{

			if(blocksPrimary->next != NULL){
				//primary is at the far right end
				//update flists to contain the prev of primary as head
				if(blocksPrimary->next->next == NULL){
					blocksPrimary->prev->next = NULL;
					flists[lvl] = blocksPrimary->prev;
				}
				//primary is in the middle
				else{
					blocksPrimary->prev->next = blocksPrimary->next->next;
					blocksPrimary->next->next->prev = blocksPrimary->prev;	
				}
			}
			else{
				blocksPrimary->prev->next = NULL;
				flists[lvl] = blocksPrimary->prev;
			}	
		}
}

void Insert(struct head*blocksPrimary){

	if(flists[blocksPrimary->level]==NULL){
		
		flists[blocksPrimary->level] = blocksPrimary;
	}
	else{
	
		flists[blocksPrimary->level]->next = blocksPrimary;
		blocksPrimary->prev = flists[blocksPrimary->level];
		flists[blocksPrimary->level] = blocksPrimary;
	}
}

void bfree(void* memory) {
	
	if(memory == NULL){
		return;
	}
	//unhide the header
	struct head* block = magic(memory);
	block->status = Free;

	//next step is to check if merging is possible
	int lvl = block->level;
	//get the buddy as well as the primary
	struct head* blocksBuddy;
	struct head* blocksPrimary;
	struct head*temp;
	
	//if the level is maximum we dont have a buddy 
	//check for this case
	if(lvl < 7){
		blocksBuddy = buddy(block);
		blocksPrimary= primary(blocksBuddy);
		//used for keeping track of the prev level primary in merge
		temp= blocksPrimary;
	}
	else{
		return;
	}
	 

	
	//////////////////MERGING/////////////////
	//check buddy status->if free->unlink->find primary->insert to next level
	while(lvl < 6){
	
		//if buddy is not free break
		//no merging possible
		if(blocksBuddy->status == Taken){
			break;
		}
	

	
		///////////////////////////////UNLINKING///////////////////////////////////////////
		
		Unlink(blocksPrimary,lvl);
		/////////////////////////////////////////////////////////////////////////////////
	
		//update primarys block level and get its next level buddy
		blocksPrimary->level +=1;
		
		blocksBuddy = buddy(blocksPrimary);
		
		
		//remove links...unlink the primary
		blocksPrimary->next = NULL;
		blocksPrimary->prev = NULL;
		
		
		
		// if the buddy is in the same level as  primary->level-1
		// the NEXT level in flists does NOT contain the buddy
		//if the buddy is taken we can just link them
		//return cause we cant merge anymore..,after insert
		if(blocksBuddy->status == Taken || blocksBuddy->level <= blocksPrimary->level-1){
			
			//add the primary to next level 
			Insert(blocksPrimary);
			return;
			
		}
		//we can merge
		else{
			//the buddy is FREE 
			//keep track of prev primary with temp and get the next level primary
			temp=blocksPrimary;
			blocksPrimary = primary(blocksBuddy);
			
			//insert the primary of the next level before the buddy...Makes it easier

			//we know that the prev primary is the primary of the next level
			if(temp == blocksPrimary){
				
				if(blocksBuddy->prev == NULL){
					
					blocksPrimary->next = blocksBuddy;
					blocksBuddy->prev = blocksPrimary;
					blocksPrimary->prev = NULL;
				}
				else{
					
					blocksBuddy->prev->next = temp;
					temp->prev = blocksBuddy->prev;
					temp->next = blocksBuddy;
					blocksBuddy->prev = temp;
				}
				
			}	
			//the buddy in the next level is primary
			else{
				
				//update the linked list such that the buddy comes first
				if(blocksBuddy->next == NULL){
					
					blocksBuddy->next = temp;
					temp->prev = blocksBuddy;
					temp->next = NULL;
					flists[blocksBuddy->level] = temp;	
					
				}
				else{
					
					blocksBuddy->next->prev = temp;
					temp->next = blocksBuddy->next;
					blocksBuddy->next = temp;
					temp->prev = blocksBuddy;
				}
			}
		}
	
		//merge down the list from 0->6
		lvl = lvl + 1;
		
	}
	
	//Last merge if possible
	
	blocksBuddy = buddy(blocksPrimary);
	
	blocksPrimary = primary(blocksBuddy);
	

	// //if the pair is free 
	// And if only the buddy level is MAX-1 i.e 6 for our implementation
	// //we can add to last level of the list
	// //same steps as in the merge
	// //unlink->check next level-> insert 
	
	if(blocksPrimary->status == Free && blocksBuddy->level==6){
		
		//unlink
		Unlink(blocksPrimary,lvl);

		blocksPrimary->level += 1;
		blocksPrimary->next = NULL;
		blocksPrimary->prev = NULL;

		//insert
		Insert(blocksPrimary);
		
	}

	
}

