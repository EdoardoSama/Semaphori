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
  
  SemDescriptorPtr* semptr = SemDescriptorList_byFd(semdes->ptr, res->id);
	if(!semptr){
    printf("Errore: SemDescriptor not found\n");
		running->syscall_retvalue=-1;
		return;
  }
  while(semptr){
		if(semptr->descriptor->fd==semdes->fd) break;
		else semptr=(SemDescriptorPtr*)semptr->list.next;
			}
  
  List_detach(&res->descriptors, (ListItem*) semdes->ptr);
  List_detach(&running->sem_descriptors, (ListItem*)semdes);
  SemDescriptorPtr_free(semptr);
	SemDescriptor_free(semdes);

  if(res->descriptors.size == 0 && res->waiting_descriptors.size == 0){
		printf("Semaphore %d will be deleted\n", res->id);
		List_detach(&semaphores_list, (ListItem*) res);
		Semaphore_free(res);
	}

  List_detach(&(semaphores_list), res);
  }
