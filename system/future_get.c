/*
* future_get
* created by Nawaz
*/
#include <future.h>
#include <myq.h>
/*---------------------
* future_get get the value of a future set by
* an operation and may change the state of the future
* pararms:
*	future_t* f:	Future on which to wait for and obtain value
*	char* value:	Address into which the future’s value will be copied.
* return type:
*	syscall:                SYSERR orr OK
----------------------*/

syscall future_get(future* f, char* value){
	intmask mask;
	mask = disable();
	pid32 pid,status;
	
	f->consumer_count = f->consumer_count + 1;	
	if(f->flags == FUTURE_EXCLUSIVE){
		//f->consumer_count = f->consumer_count + 1;
		if(f->consumer_count <= 1){// allowing only one consumer
			if(f->state == FUTURE_FULL){
				// get the value if the future is full.
				*value = *f->value;
				f->state = FUTURE_EMPTY;
			}else{
				// Future is not set with a new value, suspend the process
				f->pid = getpid();
				f->state = FUTURE_WAITING;
				pid = f->pid;
				suspend(pid);
				*value = *f->value;
			}
			restore(mask);
                        return OK;
		}else{
			// return error for multiple consumers
			restore(mask);
                        return SYSERR;
		}
	}else if(f->flags == FUTURE_SHARED){
			fprintf(stderr,"---in Future_get---\n");
		if(f->state == FUTURE_FULL){
			// get the value if the	future is full.
                        *value = *f->value;
			f->state = FUTURE_EMPTY;
				fprintf(stderr,"f->value is %d\n",value);
			
    			
			if(fut_isempty(f->get_queue)){// mark future as empty if there are no consumers waiting
				f->state = FUTURE_EMPTY;
					fprintf(stderr,"f marked as empty\n");
			}
		}else{// Future is not full
			f->state=FUTURE_WAITING;
			pid = fut_enqueue(f->pid, f->get_queue);// add it to the get_queue and suspend it
			if(pid == (pid32) SYSERR){
				restore(mask);
				return SYSERR;
			}
			//f->state = FUTURE_WAITING;
			suspend(f->pid);
			
			*value=*f->value;
			
			if(fut_isempty(f->get_queue)){// mark future as	empty if there are no consumers	waiting
                                f->state = FUTURE_EMPTY;
                        }
		}
		restore(mask);
                return OK;
	}else if(f->flags == FUTURE_QUEUE){
		if(!fut_isempty(f->set_queue)){// resume threads one by one
			pid = fut_dequeue(f->set_queue);
			resume(pid);
		}else{//If there no queued producers enqueue producers
			pid = getpid();
			// add currpid to get queue
			status = fut_enqueue(pid, f->get_queue);
			if(status == (pid32)SYSERR){
				restore(mask);
				return SYSERR;
			}
			suspend(pid);
			*value=*f->value;
		}
		
		restore(mask);
                return OK;
        }
	restore(mask);
	return SYSERR;
}
