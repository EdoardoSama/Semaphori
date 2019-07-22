#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  //1 retrieve the fd of the resource to close
  int fd=running->syscall_args[0];

  SemDescriptor* des=SemDescriptorList_byFd(&running->sem_descriptors, fd);
  //2 if the fd is not in the the process, we return an error
  if (! des){
    running->syscall_retvalue=-1;
    return;
  }

  //3 we remove the SemDescriptor from the process list
  des = (SemDescriptor*) List_detach(&running->sem_descriptors, (ListItem*) des);
  assert(des);

  Semaphore* res=des->ptr;

  // we remove the SemDescriptor pointer from the resource list
  SemDescriptorPtr* desptr=(SemDescriptorPtr*) List_detach(&res->descriptors, (ListItem*)(des->ptr));
  assert(desptr);
  SemDescriptor_free(des);
  SemDescriptorPtr_free(desptr);
  running->syscall_retvalue=0;
}
