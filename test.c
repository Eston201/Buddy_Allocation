#include "buddy.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "buddy.c"
int main(){

void *init = sbrk(0);
void *current;
printf("The initial top of the heap is %p.\n", init);

struct head* block = new();
blockinfo(block);

current = sbrk(0);
int allocated = (int)((current - init) / 1024);
printf("The current top of the heap is %p. \n", current);
printf("increased by %d Kbyte\n", allocated);

struct head* bud = buddy(block);
// blockinfo(bud);

struct head* splitted = split(block);
blockinfo(splitted);

struct head* primes = primary(splitted);
blockinfo(primes);
struct head* primeb = primary(block);
// blockinfo(primeb);

struct head* hiddenblockhead = hide(block);
blockinfo(hiddenblockhead);

struct head* unhiddenblockhead = magic(hiddenblockhead);
blockinfo(unhiddenblockhead);

int leveltest1 = level(32);
printf("min required level = %d\n",leveltest1);
int leveltest2 = level(100);
printf("min required level = %d\n",leveltest2);
int leveltest3 = level(1500);
printf("min required level = %d\n",leveltest3);


/////////////////////////OWN TESTS////////////////////////////////////////////////////////////////////////////

//crating multiple allocations

//TEST 0:
//creating blocks of level 0
    // struct head*all = balloc(5);
    // struct head*all2 = balloc(5);
    // struct head*all3 = balloc(8);
    // struct head*all4 = balloc(6);
    // printflists();

//TEST 1:
//     struct head*all = balloc(40);
//     struct head*all2 = balloc(40);
//     struct head*all3 = balloc(40);
//     struct head*all4 = balloc(40);
//     struct head*all5 = balloc(40);
//     struct head*all6 = balloc(40);
//     struct head*all7= balloc(40);
//     struct head*all8 = balloc(40);
//     struct head*all9 = balloc(40);
//     struct head*all10 = balloc(40);
//     struct head*all11 = balloc(40);
//     struct head*all12 = balloc(40);
//     struct head*all13 = balloc(40);
//     struct head*all14 = balloc(40);
//     struct head*all15 = balloc(40);
//     struct head*all16 = balloc(40);
//     struct head*all17 = balloc(40);
//     struct head*all18 = balloc(40);
//     struct head*all19 = balloc(40);
//     struct head*all20 = balloc(40);
//     struct head*all21 = balloc(40);
//     struct head*all22 = balloc(40);
//     struct head*all23 = balloc(40);
//     struct head*all24 = balloc(40);
//     struct head*all25 = balloc(40);
//     struct head*all26 = balloc(40);
//     struct head*all27 = balloc(40);
//     struct head*all28 = balloc(40);

// //counting the num of allocation in flist lvl 1
//     struct head*c = getnode(1);
//     int count = 0;
//     if(c != NULL){
//         count = 1;
//     }
//     while(c->prev != NULL){
//         count++;
//         c = c->prev;
       
//     }
 
// //printing the list
//     printflists();
//     printf("%d\n",count);
    


//TEST 2:

    // struct head*all = balloc(1500);
    // struct head*all2 = balloc(32);
    // struct head*all3 = balloc(40);
    // struct head*all4 = balloc(10);
    // printflists();
    

//TEST 3:
    // struct head*all = balloc(32);
    // struct head*all2 = balloc(64);
    // struct head*all3 = balloc(128);
    // struct head*all4 = balloc(256);
    // struct head*all5 = balloc(512);
    // struct head*all6 = balloc(1024);
    // struct head*all7= balloc(2048);
    // struct head*all8 = balloc(1);
   
    // printflists();


//TEST 4:
    // struct head*all = balloc(40);
    // struct head*all2 = balloc(40);
    // struct head*all3 = balloc(32);
    // struct head*all4 = balloc(40);
    // struct head*all5 = balloc(40);
    // struct head*all6 = balloc(500);
    // struct head*all7= balloc(4000);
    // printflists();

//TEST 5:
    // struct head*all = balloc(4000);
    // struct head*all2 = balloc(4000);
    // struct head*all3 = balloc(32);
    // struct head*all4 = balloc(40);
    // struct head*all5 = balloc(40);
    // struct head*all6 = balloc(500);
    // struct head*all7= balloc(4000);
    // printflists();

//TEST 6
    // struct head*all = balloc(40);
    // struct head*all2 = balloc(40);
    // struct head*all3 = balloc(1500);
    
    // printflists();

//TEST 7:
    // struct head*all = balloc(500);
    // struct head*all2 = balloc(40);
    // printflists();


/////////////////////////BFREE///////////////
//freeing allocations 

//TEST 0:
    // struct head*blocks [64] = {NULL};
    // for(int i=0 ; i < 64; i ++){
    //     struct head*temp = balloc(40);
    //     blocks[i] = temp;
    // }
    // //check the allocations
    // // printflists();
    // //freeing them
    // for(int i=0 ; i < 64 ; i++){
    //     bfree(blocks[i]);
    // }
    // printflists();

//TEST 1:
    // struct head*all = balloc(40);
    // bfree(all);
    // printflists();

//TEST 2:
    // struct head*all = balloc(40);
    // struct head*all2 = balloc(40);
    // struct head*all3 = balloc(40);
    // struct head*all4 = balloc(40);
    // bfree(all);
    // bfree(all2);
    // bfree(all3);
    // bfree(all4);
    // printflists();

//TEST 3:

    // struct head*all = balloc(64);
    // struct head*all2 = balloc(40);
    // bfree(all);
    
    // printflists();
    

//TEST 4:
    // struct head*all = balloc(40);
    // struct head*all2 = balloc(40);
    // struct head*all3 = balloc(32);
    // struct head*all4 = balloc(40);
 
    // bfree(all3);
    // bfree(all);
    // bfree(all2);
    // bfree(all4);

    // printflists();


//TEST 5:
    // struct head*all = balloc(100);
    // struct head*all2 = balloc(100);
    // struct head*all3 = balloc(32);
    // bfree(all3);
    // printflists();

//TEST 6:
    // struct head*all = balloc(40);
    // struct head*all2 = balloc(32);
    // struct head*all3 = balloc(32);
    // struct head*all4 = balloc(2000);
    // struct head*all5 = balloc(100);
    // struct head*all6 = balloc(60);
    // bfree(all3);
    // bfree(all2);
    // bfree(all5);
    // bfree(all6);
    // bfree(all);
    // bfree(all4);
    // printflists();

//TEST 7:
    // struct head*all = balloc(40);
    // struct head*all2 = balloc(40);
    // struct head*all3 = balloc(40);
    // struct head*all4 = balloc(40);
    // struct head*all5 = balloc(40);
    // struct head*all6 = balloc(40);
    // struct head*all7= balloc(40);
    // struct head*all8 = balloc(40);
    // struct head*all9 = balloc(40);
    // struct head*all10 = balloc(40);
    // struct head*all11 = balloc(40);
    // struct head*all12 = balloc(40);
    // struct head*all13 = balloc(40);
    // struct head*all14 = balloc(40);
    // struct head*all15 = balloc(40);
    // struct head*all16 = balloc(40);
    // struct head*all17 = balloc(40);
    // struct head*all18 = balloc(40);
    // struct head*all19 = balloc(40);
    // struct head*all20 = balloc(40);
    // struct head*all21 = balloc(40);
    // struct head*all22 = balloc(40);
    // struct head*all23 = balloc(40);
    // struct head*all24 = balloc(40);
    // struct head*all25 = balloc(40);
    // struct head*all26 = balloc(40);
    // struct head*all27 = balloc(40);
    // struct head*all28 = balloc(40);
    // bfree(all);
    // bfree(all2);
    // bfree(all3);
    // bfree(all4);
    // bfree(all5);
    // bfree(all6);
    // bfree(all7);
    // bfree(all8);
    // bfree(all9);
    // bfree(all10);
    // bfree(all11);
    // bfree(all12);
    // bfree(all13);
    // bfree(all14);
    // bfree(all15);
    // bfree(all16);
    // bfree(all17);
    // bfree(all18);
    // bfree(all19);
    // bfree(all20);
    // bfree(all21);
    // bfree(all22);
    // bfree(all23);
    // bfree(all24);
    // bfree(all25);
    // bfree(all26);
    // bfree(all27);
    // bfree(all28);
    // printflists();


//TEST 8:
    // struct head*blocks [64] = {NULL};
    // for(int i=0 ; i < 64; i ++){
    //     struct head*temp = balloc(100);
    //     blocks[i] = temp;
    // }

    // for(int i=0 ; i < 64 ; i++){
    //     bfree(blocks[i]);
    // }
    // printflists();



//ALL Other tests used when coding balloc and bfree


    //printf("%d\n",sizeof(struct head));
    // struct head*temp = new();
    // struct head*t = split(temp);
    // t->level = temp->level = temp->level-1;

    // struct head*bu = buddy(temp);
    // struct head*b = buddy(t);
    // blockinfo(bu);
    // blockinfo(b);



    // struct head*temp = new();
    // struct head*t = buddy(temp);
    // if(t==NULL){
    //     printf("YESs\n");
    // }
    // else{
    //     printf("NOO\n");
    // }
    // struct  head*t = magic(all);
    // struct  head*t1 = magic(all2);
    // blockinfo(t);
    // struct head*bud = buddy(t1);
    // blockinfo(bud);
    // struct head*temp = primary(t1);
    // blockinfo(temp);
    
    
    
    // struct head*bud = buddy(all);
    // blockinfo(bud);
    // blockinfo(bud);
    // blockinfo(all2);
    // struct head*all3 = balloc(400);
    // struct head*temp = magic(all);
    // blockinfo(temp);
    // printflists();

    // bfree(all3);
    // printflists();
    // bfree(all);
    // struct head*temp = balloc(40);
    // bfree(all4);
    // bfree(all5);
    // printflists();
    // bfree(all2);
    //  bfree(all3);
   
    
    // struct head*temp = new();
    // struct head*temps = split(temp);
    // temps->level = temp->level = temp->level-1;

    // 
    // struct head*temp = new();
    // blockinfo(temp);
    // struct head*t = split(temp);
    // blockinfo(temp);

    return 0;
}
