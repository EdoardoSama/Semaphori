#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int fd=running->syscall_args[0];
  
  //acchiappo il semaforo
  Semaphore* res = SemaphoreList_byId(&(semaphores_list), fd);
  if(!res){
    printf("Errore: semaforo not found\n");
		running->syscall_retvalue=-1;
		return;
  }
   //acchiappo il descrittore
  SemDescriptor* semdes = (SemDescriptor*)running->sem_descriptors.first;
	if(!semdes){
    printf("Errore: SemDescriptor not found\n");
		running->syscall_retvalue=-1;
		return;
  }
  while(semdes){
    if(semdes->semaphore->id==res->id) break;
    else semdes=(SemDescriptor*)semdes->list.next;
  }
  if(semdes==NULL){
    printf("Errore: semaphoro non associato al processo\n");
		running->syscall_retvalue=-1;
    return;
  }
  
  //acchiappo il puntatore a descrittore
  SemDescriptorPtr* semptr = (SemDescriptorPtr*)res->descriptors.first;
	if(!semptr){
    printf("Errore: SemDescriptorPtr not found\n");
		running->syscall_retvalue=-1;
		return;
  }
  while(semptr){
		if(semptr->descriptor->fd==semdes->fd) break;
		else semptr=(SemDescriptorPtr*)semptr->list.next;
	}
  if(semptr==NULL){
    printf("Errore: \n");
		running->syscall_retvalue=-1;
    return;
  }
  
  //decremento il contatore del semaforo, è quello che fa il semwait
  if(res->count > 0) res->count--;
  //se il contatore invece è a 0, stacco il puntatore a descrittore dal semaforo, e lo metto nella lista
  //in waiting, sempre appartenente al semaforo
  else{
    List_detach(&res->descriptors, (ListItem*) semptr);
    List_insert(&res->waiting_descriptors, res->waiting_descriptors.last, (ListItem*) semdes->ptr);
    //metto il processo corrente in coda alla waiting list
    List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
    running->status = Waiting;
    //metto il primo processo della readylist in running
    PCB* pcb = (PCB*) List_detach(&ready_list, (ListItem*) ready_list.first);
    running = (PCB*)pcb;
  }

  running->syscall_retvalue=0;
  return;
}

