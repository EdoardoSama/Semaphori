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
  //1 retrieve the fd of the resource to close
  int fd=running->syscall_args[0];


  Semaphore* res = SemaphoreList_byId(&(semaphores_list), fd);
  if(!res){
    printf("Errore: semaforo non in lista\n");
		running->syscall_retvalue=-1;
		return;
  }

  SemDescriptor* semdes = (SemDescriptor*)running->sem_descriptors.first;
  if(!semdes){
    printf("Errore: descrittore non trovato\n");
		running->syscall_retvalue=-1;
		return;
  }
  while(semdes != NULL){
    SemDescriptorPtr* semptr = (SemDescriptorPtr*) semdes->ptr;
    if(!semptr){
      printf("Errore: puntatore a descrittore non trovato\n");
      running->syscall_retvalue=-1;
      return;
      }
      List_detach(semptr, (ListItem*)semptr->descriptor);
      semdes->list.next;
    } 
  }
