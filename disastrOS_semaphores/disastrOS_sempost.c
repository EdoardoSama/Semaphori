#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
	int semnum = running->syscall_args[0];
	
	SemDescriptor* semdesc = (SemDescriptor*)SemDescriptorList_byFd(&running->sem_descriptors, semnum);
			if(!semdesc){
				running->syscall_retvalue=-1;
				return;
				}

	//acchiappo il semaforo
	Semaphore* res = semdesc->semaphore;
	//incremento il contatore, è quello che fa il sempost
	printf("\nsem_post invocata su semaforo #%d count:%d verrà incrementato\n", semnum+1, res->count);
	(res->count)++;

	//se il contatore è a 0 (o meno), pesco il primo puntatore a descrittore dalla lista di waiting,
	//e lo schiaffo per ultimo nella lista di quelli "attivi"
	if(res->count <= 0) { 
			
			//metto il puntatore del descrittore nella ready list
			SemDescriptorPtr* semptr = (SemDescriptorPtr*)List_detach(&res->waiting_descriptors,res->waiting_descriptors.first);
			List_insert(&res->descriptors, res->descriptors.last, (ListItem*)semptr);

			//tolgo il PCB dalla waiting list e la sparo in ready
			PCB* pcb = semptr->descriptor->pcb;
			pcb->status = Ready;

			List_detach((ListHead*)&waiting_list, (ListItem*)pcb);
			List_insert((ListHead*)&ready_list, (ListItem*)ready_list.last, (ListItem*)pcb);
			
		}
  running->syscall_retvalue=0;
  return;
  }

