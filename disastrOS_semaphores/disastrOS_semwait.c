#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int fd=running->syscall_args[0];
  
  Semaphore* res = SemaphoreList_byId(&(semaphores_list), fd);
  if(!res){
    printf("Errore: semaforo not found\n");
		running->syscall_retvalue=-1;
		return;
  }

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
}
