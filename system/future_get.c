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
	/*
	If in the flag od Future exclusive, allow only one consumer
	*/
	//f->consumer_count = f->consumer_count + 1;	
	if(f->flags == FUTURE_EXCLUSIVE){
		f->consumer_count = f->consumer_count + 1;
		if(f->consumer_count <= 1){// allowing only one consumer
			if(f->state == FUTURE_FULL){
				// get the value if the future is full.
				*value = *f->value;
				f->state = FUTURE_EMPTY;
				//fprintf(stderr,"%s: %d \r\n", __FUNCTION__, __LINE__);
			}else{
				// Future is not set with a new value, suspend the process
				f->pid = getpid();
				f->state = FUTURE_WAITING;
				pid = f->pid;
				//fprintf(stderr,"2 %s: %d \r\n", __FUNCTION__, __LINE__);
				suspend(pid);
				//fprintf(stderr,"3 %s: %d \r\n", __FUNCTION__, __LINE__);
				*value = *f->value;
			}
			restore(mask);
                        return OK;
		}else{
			// return error for multiple consumers
			restore(mask);
                        return SYSERR;
		}
	/*
        If in the flag is Future shared, one to many invocation
        */
	}else if(f->flags == FUTURE_SHARED){
		//fprintf(stderr,"---in Future_get---\n");
		if(f->state == FUTURE_FULL){
			// get the value if the	future is full.
                        f->state = FUTURE_EMPTY;
			*value = *f->value;
			//fprintf(stderr,"f->value is %d\n",value);
		}else if(f->state == FUTURE_EMPTY){// Future is not full
			//fprintf(stderr,"future in get: not full\n",value);
			//fprintf(stderr,"future state : %d\n",f->state);
			f->state=FUTURE_WAITING;
			status = fut_enqueue(currpid, f->get_queue);
			// add it to the get_queue and suspend it. Will be resumed by producer
			if(status == (pid32) SYSERR){
				restore(mask);
				return SYSERR;
			}
			suspend(currpid);			
			*value=*f->value;
			restore(mask);
                	return OK;
		}else if(f->state == FUTURE_WAITING){
			//fprintf(stderr,"future state : %d\n",f->state);
			status = fut_enqueue(currpid, f->get_queue);// add it $
                        if(status == (pid32) SYSERR){
                                restore(mask);
                                return SYSERR;
                        }
			suspend(currpid);
			*value = *f->value;
			if(fut_isempty(f->get_queue)){// mark future as empty if no one is waiting on it.
                                f->state = FUTURE_EMPTY;
                        	//fprintf(stderr,"ffff marked as empty\n");
                        }
			restore(mask);
                	return OK;			
		}
	/*
	If in the flag is Future Queue: many to many invocation
        */
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
