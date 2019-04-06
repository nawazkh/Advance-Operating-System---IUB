/*
* Queue operations
* Enqueue
* Dequeue
* isEmpty
* isFull
*/
#include <xinu.h>
#include <myq.h>

/* -----------------
* fut_isempty :  check if queue is empty
* input : queue to check
* ---------------*/
int fut_isempty(myq* queue){
  if(queue->size == 0){
    return 1;
  }
  return 0;
}

/* -----------------
* fut_isfull :  check if queue is full
* input : queue to check
* ---------------*/
int fut_isFull(myq* queue){
  return (queue->size == QUEUE_SIZE);
}

/* -----------------
* fut_enqueue :  enqueue process id
* input : pid and the queue to insert
* ---------------*/
pid32 fut_enqueue(pid32 pid, myq* queue){
  if(fut_isFull(queue)){
    return (pid32) SYSERR;
  }
  queue->rear = (queue->rear + 1) % QUEUE_SIZE;
  queue->parray[queue->rear] = pid;
  queue->size=queue->size+1;
  return pid;
}

/* -----------------
* fut_dequeue :  dequeue process id from front
* input : queue to dequeue from the end
* ---------------*/
pid32 fut_dequeue(myq* queue){
  if(fut_isempty(queue)){
    return (pid32) SYSERR;
  }
  pid32 pid;
  pid = queue->parray[queue->front];
  queue->front = (queue->front + 1) % QUEUE_SIZE;
  queue->size = queue->size - 1;
  return pid;
}

/* -----------------
* fut_peekFirst : peek first element
* input : queue
* ---------------*/
pid32 fut_peekFirst(myq* queue){
  if(fut_isempty(queue)){
    return (pid32) SYSERR;
  }
  return queue->parray[queue->front];
}

/* -----------------
* fut_peekLast : peek first element 
* input : queue
* ---------------*/
pid32 fut_peekLast(myq* queue){
  if(fut_isempty(queue)){
    return (pid32) SYSERR;
  }
  return queue->parray[queue->rear];
} 

