#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int semnum=running->syscall_args[0];
  
  if(semaphores_list.size==0){
		running->syscall_retvalue=-1;
		return;
	}
   
   //acchiappo il descrittore
  SemDescriptor* semdes = SemDescriptorList_byFd(&running->sem_descriptors, semnum);
	if(!semdes){
		running->syscall_retvalue=-1;
		return;
  }
  
  //acchiappo il semaresetreforo
  Semaphore* res = semdes->semaphore;
  
  //acchiappo il puntatore a descrittore
  SemDescriptorPtr* semptr = (SemDescriptorPtr*)semdes->ptr;
	if(!semptr){
    printf("\nErrore: SemDescriptorPtr non trovato (invocazione SemWait)\n");
		running->syscall_retvalue=-1;
		return;
  }
  //decremento il contatore del semaforo, è quello che fa il semwait
  printf("\nsem_wait invocata su semaforo #%d count:%d verrà decrementato\n", semnum+1, res->count);
  res->count--;
  //se il contatore è a 0, stacco il puntatore a descrittore dal semaforo, e lo metto nella lista
  //in waiting, sempre appartenente al semaforo
  if(res->count < 0){
    List_detach(&res->descriptors, (ListItem*) semptr);
    List_insert(&res->waiting_descriptors, res->waiting_descriptors.last, (ListItem*) semptr);
    //SemaphoreList_print(&res->waiting_descriptors);
    
    //metto il semaforo nella waiting list globale
    List_insert(&waiting_list,(ListItem*)waiting_list.last,(ListItem*)res);

    //metto il processo corrente in coda alla waiting list globale
    running->status = Waiting;
    List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
    
    //metto il primo processo della readylist in running
    PCB* pcb = (PCB*) List_detach(&ready_list, (ListItem*) ready_list.first);
    running = (PCB*)pcb;
  }
  running->syscall_retvalue=0;
  return;
}

