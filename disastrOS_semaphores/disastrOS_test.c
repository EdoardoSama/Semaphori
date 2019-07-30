#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include "disastrOS.h"
#include "disastrOS_globals.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

int buffer[20]={0};
int in_idx=0;
int out_idx=0;


//paradigma n producer n consumer, Sistemi di Calcolo 2 (rinfrescato da Wikipedia)
void producer(int* buffer, int fd_mutex, int fd_fill, int fd_empty){
  
  for(int i=0; i<10; i++){
    disastrOS_semWait(fd_empty);
    disastrOS_semWait(fd_mutex);

    buffer[in_idx] = i+1;
    in_idx = (in_idx+1)%20;
    printf("\nIl processo %d (PRODUCER) accede al buffer\n",disastrOS_getpid());
    disastrOS_sleep(disastrOS_getpid()); 

    disastrOS_semPost(fd_mutex);
    disastrOS_semPost(fd_fill); 

  }
}

void consumer(int* buffer, int fd_mutex, int fd_fill, int fd_empty){

  for(int i=0; i<10; i++){
    disastrOS_semWait(fd_fill); 
    disastrOS_semWait(fd_mutex);

    buffer[out_idx] = 0;  
    out_idx = (out_idx+1)%20;
    disastrOS_sleep(disastrOS_getpid());
    printf("\nIl processo %d (CONSUMER) accede al buffer\n",disastrOS_getpid());
    disastrOS_semPost(fd_mutex);
    disastrOS_semPost(fd_empty); 

  }
}

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  //printf("PID: %d, terminating\n", disastrOS_getpid());

  int mutex = disastrOS_semOpen(1, 1); 
  int fill = disastrOS_semOpen(2, 0); 
  int empty = disastrOS_semOpen(3, 20); 

  
  disastrOS_sleep(10-disastrOS_getpid());

  if (disastrOS_getpid()%2==0){
    printf("\nIl processo #%d è PRODUTTORE\n", running->pid);
    producer(buffer, mutex, fill, empty);
  }
  else{
    printf("\nIl processo #%d è CONSUMATORE\n", running->pid);
    consumer(buffer, mutex, fill, empty);
  }

  disastrOS_semClose(1);
  disastrOS_semClose(2);
  disastrOS_semClose(3);

  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<10; ++i) {
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  printf("shutdown!\n");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
