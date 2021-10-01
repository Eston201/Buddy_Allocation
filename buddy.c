
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

	if(req <= 32){
		return 0;
	}

	else if((req > 32) && (req <= 64)){
		return 1;
	}

	else if((req > 64) && (req <= 128)){
		return 2;
	}

	else if((req > 128) && (req <= 256)){
		return 3;
	}

	else if((req > 256) && (req <= 512)){
		return 4;
	}

	else if((req > 512) && (req <= 1024)){
		return 5;
	}

	else if((req > 1024) && (req <= 2048)){
		return 6;
	}

	else{
		return 7;
	}
	
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
		
		if(flists[i] != NULL){

			printf("===========================%d=======================================\n",i);
			struct head*temp = flists[i];

			blockinfo(temp);
			while(temp->prev != NULL){
				temp = temp->prev;
				blockinfo(temp);
			}
		
		}
			
	}
	
	
}

//gets a block at a certain level in the free lists
void* getnode(int n){
	return flists[n];
}


//Complete the implementation of balloc here

void* balloc(size_t size) {
	
	if(size == 0){
		return NULL;
	}
	//get the required level
	
	int lvl = level(size);
	
	//keeping track of which level we are on 
	int Ulvl = 7;
	//the block of memory to be returned
	struct head*unallocated = NULL;
	struct head*Hideunallocated;
	
	//check if a free block is in the requested block level
	if(flists[lvl] != NULL){
		
		//the top of the flists has a free block return it
		if(flists[lvl]->status == Free){

			flists[lvl]->status = Taken;
			Hideunallocated = hide(flists[lvl]);
			return Hideunallocated;
		}
		else{
			//searching the linked list at that req level for a free block
			struct head*temp = flists[lvl];
			while(temp->prev != NULL){
				temp = temp->prev;
				//if that block is free  update status hide and return
				if(temp->status == Free){
					temp->status = Taken;
					Hideunallocated = hide(temp);
					return Hideunallocated;
				}
			}
		}
	}

	//if the above fails find the next free block level in the flists
	for(int i = lvl+1 ; i < 8 ; i ++){
	
		if(flists[i] != NULL){
			if(flists[i]->status == Free){
				
				Ulvl = i;
				unallocated = flists[i];
				break;
			}
			else{
				struct head*temp = flists[i];

				while(temp->prev != NULL){

					temp = temp->prev;
					//if that block is free assign it
					if(temp->status == Free){
						
						Ulvl = i;
						unallocated = temp;
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
						break;
					}
				}
			}
		}
		
	}
	
	//if the above for loop fails we dont have any free blocks
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
	Hideunallocated = hide(unallocated);
	return Hideunallocated;
}


//Complete the implementation of bfree here
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
	//check buddy status->if free->unlink->find primary->insert to next levl
	while(lvl < 6){
	
		//if buddy is not free break
		//no merging possible
		if(blocksBuddy->status == Taken){
			break;
		}
		

		//unlink the block and buddy from its list
		//i.e unlink the primary block 
		//NOTE code struct such that the primary will always be left of the buddy

		///////////////////////////////UNLINKING///////////////////////////////////////////

		//primary is at the far left
		if(blocksPrimary->prev == NULL){

			if(blocksPrimary->next != NULL){
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

		
		//update primarys block level and get its next level buddy
		blocksPrimary->level +=1;
		
		blocksBuddy = buddy(blocksPrimary);
		
		
		//remove links
		blocksPrimary->next = NULL;
		blocksPrimary->prev = NULL;
		
		
		// if the buddy is in the same level as  primary->level-1
		// the NEXT level in flists does NOT contain the buddy
		//if the buddy is taken we can just link them
		if(blocksBuddy->status == Taken && blocksBuddy->level == blocksPrimary->level-1){
			
			//add the primary to next level 
			
			if(flists[blocksPrimary->level]==NULL){
				flists[blocksPrimary->level] = blocksPrimary;
			}
			else{
			
				flists[blocksPrimary->level]->next = blocksPrimary;
				blocksPrimary->prev = flists[blocksPrimary->level];
				flists[blocksPrimary->level] = blocksPrimary;
			}
			
		}
		//the buddy is Free but the buddys buddy is split up we cannot merge anymore and we return after adding the primary to the next level
		else if(blocksBuddy->level == blocksPrimary->level-1){
			
			if(flists[blocksPrimary->level]==NULL){
				flists[blocksPrimary->level] = blocksPrimary;
			}
			else{
			
				flists[blocksPrimary->level]->next = blocksPrimary;
				blocksPrimary->prev = flists[blocksPrimary->level];
				flists[blocksPrimary->level] = blocksPrimary;
			}
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
					temp->prev = NULL;
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
	
	
	// //if the buddy is free 
	// And if only the buddy level is MAX-1
	// //we can add to last level of the list
	// //same steps as in the merge
	// //unlink->check next level-> insert 
	if(blocksBuddy->status == Free && blocksBuddy->level==6){

		
		//unlink
		if(blocksPrimary->prev == NULL){
			if(blocksPrimary->next->next == NULL){
				flists[lvl] = NULL;
			}
			else{
				blocksPrimary->next->next->prev = NULL;
			}
		}
		else{
			if(blocksPrimary->next->next ==NULL){
				blocksPrimary->prev->next = NULL;
				flists[blocksPrimary->level] = blocksPrimary->prev;
			}
			else{
				blocksPrimary->prev->next = blocksPrimary->next->next;
				blocksPrimary->next->next->prev = blocksPrimary->prev;
			}
		}

		blocksPrimary->level += 1;
		blocksPrimary->next = NULL;
		blocksPrimary->prev = NULL;

		//insert
		if(flists[blocksPrimary->level] == NULL){
			flists[blocksPrimary->level] = blocksPrimary;
		}
		else{
	
			flists[blocksPrimary->level]->next = blocksPrimary;
			blocksPrimary->prev = flists[blocksPrimary->level];
			flists[blocksPrimary->level] = blocksPrimary;

		}	
		
	}


	
}

