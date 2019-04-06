#include <xinu.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> 
#include <myq.h>

#ifndef _FUTURE_H_
#define _FUTURE_H_

/* modes of operation for future*/
#define FUTURE_EXCLUSIVE  1	
#define FUTURE_SHARED     2	    
#define FUTURE_QUEUE      3     
 
/* define states */
#define FUTURE_FREE      0x00
#define FUTURE_EMPTY     0x01
#define FUTURE_FULL   	 0x03     
#define FUTURE_WAITING   0x05

typedef struct futent
{
  uint16 state;
  uint16 flags;
  uint32 size;
  char *value; /* alloc must memget size (in chars) space */
  pid32 pid; /* for single waiter case */
  myq* set_queue;
  myq* get_queue;
  int producer_count;
  int consumer_count;
  int prodcon_shared_count;
} future;

/* Interface for system call */
future* future_alloc(int future_flags, uint size);
syscall future_free(future*);
syscall future_get(future*, char *);
syscall future_set(future*, char *);
uint future_cons(future*);
uint future_prod(future*, int value);

#endif /* _FUTURE_H_ */
