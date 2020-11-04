#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct Node {
  size_t size;
  Node *next;
  Node *prev;
};

Node * top = NULL;
void * start = sbrk(0);

void * mmalloc(size_t size){
  if(size<=0){
    return NULL;
  }

  if(top == NULL){

    Node *space;
    int sbrkmod = 1 + (size+(sizeof(Node)))/1024;
    //printf("new malloc initiated\n");
    printf("found free space sized %d for total size %d\n",1024,(size+sizeof(Node)));

    space = (Node*)sbrk(sbrkmod * 1024);
    if(space == (Node*)-1){
      return NULL;
    }
    space->size = size;
    Node *freeAfter = (Node*)((char*)space + sizeof(Node) + size);
    top = freeAfter;

    space->next = NULL;
    space->prev = NULL;

    freeAfter->prev = NULL;
    freeAfter->next = NULL;
    freeAfter->size = (sbrkmod*1024) - (2*sizeof(Node)) - size;

    if(freeAfter->size == 0){
      space->size+=sizeof(Node);
     top = NULL;
    }

    return (void*)(space+1);
  } else {
    //printf("entered non-new malloc zone\n");
    Node * runner = top;
    bool alloced = false;
    do {

    //  printf("runner %p\n", (void*)runner);
    //  printf("runner n %p\n", (void*)runner->next);
    //  printf("runner p %p\n", (void*)runner->prev);

      //printf("entered while loop\n");
      if(runner->size >= (size+sizeof(Node))){
        printf("found free space sized %d for total size %d\n",runner->size,(size+sizeof(Node)));
        //insert and set size
        Node * newBlock;
        newBlock = (Node*)((char*)runner + size);

        if((runner->size - (size+sizeof(Node))) != 0){
          //printf("non perfect\n");
          if(runner == top){
            //printf("top switch\n");
            top = newBlock;
          }

          if(runner->prev==NULL && runner->next==NULL){
            //printf("top node changed, sole node\n");
            newBlock->prev = NULL;
            newBlock->next = NULL;
          }else if(runner->prev==NULL){
            //printf("node added to top\n");
            newBlock->prev = NULL;
            newBlock->next = runner->next;
            runner->next->prev = newBlock;
          }else if(runner->next==NULL){
            //printf("node added to bottom\n");
            newBlock->prev = runner->prev;
            runner->prev->next = newBlock;
            newBlock->next = NULL;
          }else{
            //printf("node added in moddle\n");
            newBlock->prev = runner->prev;
            runner->prev->next = newBlock;
            newBlock->next = runner->next;
            runner->next->prev = newBlock;
          }

          newBlock->size = runner->size - (size+sizeof(Node));
          runner->size = size;
        } else {
          //printf("perfect?\n");
            if(runner->prev==NULL && runner->next==NULL){
            top = NULL;
          }else if(runner->prev==NULL){
            top = runner->next;
            runner->next->prev = NULL;
          }else if(runner->next==NULL){
            runner->prev->next = NULL;
          }else{
            runner->next->prev = runner->prev;
            runner->prev->next = runner->next;
          }
        }

        alloced = true;
        //printf("allocated\n");
        return (void*)(runner+1);
      }

      if(!alloced && runner->next==NULL){ //nothing fit the data
        //printf("entered nofit data section\n");

        int sbrkmod = 1 + (size+(sizeof(Node)))/1024;
        Node * addSpace = (Node*)sbrk(sbrkmod  * 1024);

        if(addSpace == (Node*)-1){
          return NULL;
        }

        addSpace->size = size;
        Node *freeAfter = (Node*)((char*)addSpace + sizeof(Node) + size);

        addSpace->next = NULL;
        addSpace->prev = NULL;

        freeAfter->prev = runner;
        freeAfter->next = NULL;
        runner->next = freeAfter;
        freeAfter->size = (sbrkmod*1024) - (2*sizeof(Node)) - size;

        if(freeAfter->size == 0){
          addSpace->size+=sizeof(Node);
          freeAfter->prev = NULL;
          runner->next = NULL;
        }

        return (void*)(addSpace+1);
      }
      runner = runner->next;
    } while(runner != NULL);
  }
}


void mfree(void * pointer){
  if(pointer != NULL){

    Node * ptr = (Node*)pointer;
    ptr-=1;

    Node * runner = top;

    while(true){
      //printf("aaaaaaw shit\n");
      if(runner == NULL){
        //printf("nullll?\n");
        ptr->prev = NULL;
        ptr->next = NULL;
        top = ptr;
        break;
      }
      if(ptr<runner && runner->prev==NULL){
        //printf("pooooo?\n");
        runner->prev = ptr;
        ptr->prev = NULL;
        ptr->next = runner;
        top = ptr;
        break;
      }
      if(runner<ptr && runner->next==NULL){
        //printf("titties?\n");
        runner->next = ptr;
        ptr->next = NULL;
        ptr->prev = runner;
        break;
      }
      if(runner<ptr && runner->next>ptr){
        //printf("fwoooop?\n");
        ptr->next = runner->next;
        ptr->prev = runner;
        ptr->next->prev = ptr;
        ptr->prev->next = ptr;
        break;
      }
      runner = runner->next;
    }
  }
}

void printFreeList(){
  Node * runner = top;
  int rel;
  int nodec = 1;
  int tots = 0;
  printf("(prev ptr, next ptr, size)(space relative to heap)\n");
  printf("--------------------------------------------------\n");
  if (runner!=NULL) {
    do {
      rel = ((sizeof(Node))*nodec + tots)-sizeof(Node);
      tots+=runner->size;
      nodec++;
      printf("(%p,%p,%d)  (%d)\n",runner->prev,runner->next,runner->size,rel);
      runner = runner->next;
    } while(runner!=NULL);
  }
}

int main(){

  void *one, *two, *three, *four, *five, *six;

  printf("Begin testing of mmalloc\n\n");
  printf("===Free List===\n");
  printFreeList();
  printf("\n");
  printf("testing: call to sbrk() and imperfect match\n");
  one = mmalloc(163);
  printf("mmalloc returned pointer %p\n", one);

  printf("===Free List===\n");
  printFreeList();
  printf("\n");

  printf("testing: call to fill remaining space of free list\n");
  two = mmalloc(825);
  printf("mmalloc returned pointer %p\n", two);

  printf("===Free List===\n");
  printFreeList();
  printf("\n");

  printf("freeing node at %p\n", two);
  mfree(two);

  printf("===Free List===\n");
  printFreeList();
  printf("\n");

  printf("requesting smaller space for imperfect match\n");
  three = mmalloc(27);
  printf("mmalloc returned pointer %p\n", three);

  printf("===Free List===\n");
  printFreeList();
  printf("\n");

  printf("freeing pointer %p and %p\n", one, three);
  mfree(one);
  mfree(three);

  printf("===Free List===\n");
  printFreeList();
  printf("\n");

  printf("NOTE: call to size larger than 1012 works\nbut not perfect solution\n");


  return 0;
}
