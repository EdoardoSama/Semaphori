#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
	//1 get from the PCB the resource id of the resource to open
	int id_number=running->syscall_args[0];
	
	if(id_number < 0 ){
		printf("Errore: id_number negativo\n");
		running->syscall_retvalue=-1;
		return;
	}
	//controllo se il semafoto esiste già
	Semaphore* res = SemaphoreList_byId(&semaphores_list, id_number);
	
	if(res == NULL){
		//allora alloco un semaforo
		res = Semaphore_alloc(id_number, 1);
		if (res == NULL){
			printf("Errore: allocazione abortita\n");
			running->syscall_retvalue=-1;
			return;
		}
		else{
			//inserisco il semaforo nella lista
			List_insert(&semaphores_list, semaphores_list.last, (ListItem*)res);
		}
	}
	SemDescriptor* sem_des = SemDescriptor_alloc(running->last_sem_fd, res, running);
	if(sem_des == NULL){
		printf("Errore: descrittore abortito\n");
		running->syscall_retvalue=-1;
		return;
	}
	running->last_sem_fd++;
	SemDescriptorPtr* desPtr = SemDescriptorPtr_alloc(sem_des);
	List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*)sem_des);
	
	sem_des->ptr=desPtr;
	List_insert(&res->descriptors, res->descriptors.last, (ListItem*)desPtr);
	running->syscall_retvalue=id_number;
}
