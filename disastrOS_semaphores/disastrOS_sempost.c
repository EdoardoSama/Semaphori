#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
 int fd=running->syscall_args[0];
  
  //acchiappo il semaforo
  Semaphore* res = SemaphoreList_byId(&(semaphores_list), fd);
  if(!res){
    printf("Errore: semaforo not found\n");
		running->syscall_retvalue=-1;
		return;
  }
  
  //incremento il contatore, è quello che fa il sempost
  res->count++;

  //se il contatore è a 0 (o meno), pesco il primo puntatore a descrittore dalla lista di waiting,
  //e lo schiaffo per ultimo nella lista di quelli "attivi"
  if(res->count <= 0) { 
		SemDescriptorPtr* semptr = (SemDescriptorPtr*)List_detach(&res->waiting_descriptors,res->waiting_descriptors.first);
		if(!semptr){
      		printf("Errore: SemDescriptorPtr not found\n");
		  	running->syscall_retvalue=-1;
		  	return;
			}
    	
		List_insert(&res->descriptors, res->descriptors.last, (ListItem*)semptr);

		//aggiorno il pcb e lo metto in ready-mode
		PCB* pcb = semptr->descriptor->pcb;
		pcb->status = Ready;
		List_detach((ListHead*)&waiting_list, (ListItem*)pcb);
		List_insert((ListHead*)&ready_list, (ListItem*)ready_list.last, (ListItem*)pcb);
	}
  running->syscall_retvalue=0;
  return;
  }

