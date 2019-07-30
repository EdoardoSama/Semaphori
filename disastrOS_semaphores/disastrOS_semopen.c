#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
	//controlli di rito
	if(semaphores_list.size >= MAX_NUM_SEMAPHORES){
		printf("\nErrore: # di semafori eccede il massimo\n");
		running->syscall_retvalue=-1;
		return;
	}
	
	else if(running->sem_descriptors.size >= MAX_NUM_DESCRIPTORS_PER_PROCESS){
		printf("\nErrore: troppi semafori x processo\n");
		running->syscall_retvalue=-1;
		return;
	}
	else{
		int semnum=running->syscall_args[0];
		int semval=running->syscall_args[1];

		if(semnum < 0 ){
			printf("\nErrore: identificativo negativo\n");
			running->syscall_retvalue=-1;
			return;
		}
		//controllo se il semafoto esiste già
		Semaphore* res = SemaphoreList_byId(&semaphores_list, semnum);
		
		if(!res){
			//se non esiste alloco un semaforo
			res = Semaphore_alloc(semnum, semval);
			printf("\nCreazione semaforo #:%d, counter inizializzato a:%d \n", semnum, semval);
			if (!res){
				printf("\nErrore: allocazione abortita\n");
				running->syscall_retvalue=-1;
				return;
			}
			else{
				//inserisco il semaforo nella lista dei semafori
				List_insert(&semaphores_list, semaphores_list.last, (ListItem*)res);
			}
		}
		//alloco il descrittore
		SemDescriptor* semdes = SemDescriptor_alloc(running->last_sem_fd, res, running);
		if(!semdes){
			printf("\nErrore: allocazione descrittore abortito\n");
			running->syscall_retvalue=-1;
			return;
		}
		
		//alloco il puntatore al descrittore
		SemDescriptorPtr* desPtr = SemDescriptorPtr_alloc(semdes);
		
		//inserisco il semaforo nella lista dei descrittori dei semafori
		List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*)semdes);
		
		running->last_sem_fd++;
		
		semdes->ptr=desPtr;
		List_insert(&res->descriptors, res->descriptors.last, (ListItem*)desPtr);
		
		
		running->syscall_retvalue=semdes->fd;
	}
}