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

  SemDescriptor* semdes = SemDescriptorList_byFd(&running->sem_descriptors, fd);
	if(!semdes){
		running->syscall_retvalue = -1;
		return;
	}
  

  Semaphore* res = SemaphoreList_byId(&(semaphores_list), fd);
  if(!res){
    printf("Errore: semaforo non in lista\n");
		running->syscall_retvalue=-1;
		return;
  }

  SemDescriptorPtr* semptr = (SemDescriptorPtr*) semdes->ptr;
  if(!semptr){
    printf("Errore: puntatore a descrittore non trovato\n");
    running->syscall_retvalue=-1;
    return;
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
