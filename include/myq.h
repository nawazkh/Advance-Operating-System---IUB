#ifndef _F_QUEUE_H_
#define _F_QUEUE_H_

#define QUEUE_SIZE 50 

typedef  struct futqueue
{
  	int parray[QUEUE_SIZE];
  	int front;
  	int rear;
  	int size;
} myq;

#endif
