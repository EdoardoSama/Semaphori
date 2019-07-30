#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_constants.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_globals.h"

void internal_semClose(){
  
  int semnum=running->syscall_args[0];
  
  Semaphore* res = SemaphoreList_byId(&(semaphores_list), semnum);
  if(!res){
    //printf("Errore: semaforo non trovato, stavo cercando il semaforo #%d\n", semnum);
		running->syscall_retvalue=-1;
		return;
  }

  SemDescriptor* semdes = (SemDescriptor*)running->sem_descriptors.first;
	if(!semdes){
    printf("\nErrore: SemDescriptor non trovato\n");
		running->syscall_retvalue=-1;
		return;
  }
  while(semdes){
    if(semdes->semaphore->id==res->id) break;
    else semdes=(SemDescriptor*)semdes->list.next;
  }
  if(semdes==NULL){
    printf("\nErrore: semaphoro non associato al processo\n");
		running->syscall_retvalue=-1;
    return;
  }
  
  SemDescriptorPtr* semptr = (SemDescriptorPtr*)res->descriptors.first;
	if(!semptr){
    printf("\nErrore: SemDescriptorPtr non trovato\n");
		running->syscall_retvalue=-1;
		return;
  }
  while(semptr){
		if(semptr->descriptor->fd==semdes->fd) break;
		else semptr=(SemDescriptorPtr*)semptr->list.next;
	}
  if(semptr==NULL){
    printf("\nErrore: semaphoro non associato al processo\n");
		running->syscall_retvalue=-1;
    return;
  }
  
  List_detach(&res->descriptors, (ListItem*) semdes->ptr);
  List_detach(&running->sem_descriptors, (ListItem*)semdes);
  SemDescriptorPtr_free(semptr);
	SemDescriptor_free(semdes);

  if(res->descriptors.size == 0 && res->waiting_descriptors.size == 0){
		printf("\nSemaforo #%d verrà terminato\n", res->id);
		List_detach(&semaphores_list, (ListItem*) res);
		Semaphore_free(res);
	}

}
