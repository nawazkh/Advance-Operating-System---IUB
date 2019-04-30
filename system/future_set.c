/*
* future_set
* created by Nawaz
*/
#include <future.h>
#include <myq.h>

/*---------------------
* future_set set the value of a future and may change the state of the future
* pararms:
*	future_t* f:    Future on which to wait for and obtain value
*	char* value:    Address into which the future’s value will be copied.
* return type:
*	syscall:                SYSERR orr OK
----------------------*/
syscall future_set(future* f, char* value){
	pid32 pid,status;
	intmask mask;
	mask = disable();

	f->producer_count = f->producer_count + 1;
	if(f->flags == FUTURE_EXCLUSIVE){
		if(f->producer_count <= 1){
			if(f->state==FUTURE_EMPTY){
				f->value=value;
				f->state=FUTURE_FULL;
			}
			else if(f->state==FUTURE_WAITING){
				f->value=value;
                                f->state=FUTURE_FULL;
				pid = f->pid;
				//fprintf(stderr,"2 %s: %d \r\n", __FUNCTION__, __LINE__);
				resume(pid);
			}
			restore(mask);
			return OK;
		}else{
			restore(mask);
			return SYSERR;
		}
	}else if(f->flags==FUTURE_SHARED){
		f->prodcon_shared_count=f->prodcon_shared_count+1;
		//fprintf(stderr,"future prodcon shared count is %d\n",f->prodcon_shared_count);
		if(f->prodcon_shared_count <= 1){
			//fprintf(stderr,"f->state is %d\n",f->state);
			if(f->state==FUTURE_EMPTY){
				f->value=value;
				f->state=FUTURE_FULL;
				//fprintf(stderr,"f->value is %d\n",f->value);
			}
			else if(f->state==FUTURE_WAITING){
				f->value=value;
				f->state=FUTURE_FULL;
				//fprintf(stderr,"State:future waiting,f->value is %d\n",f->value);
				while(!fut_isempty(f->get_queue)){
					pid = fut_dequeue(f->get_queue);
					//fprintf(stderr,"2 %s: %d \r\n", __FUNCTION__, __LINE__);
					resume(pid);
				}
			}
			restore(mask);
                        return OK;
		}
		else{
			restore(mask);
                        return SYSERR;
		}
	}
	else if(f->flags==FUTURE_QUEUE){
		if(!fut_isempty(f->get_queue)){
			f->value=value;
			//f->pid = getpid();
			pid = fut_dequeue(f->get_queue);
			resume(pid);
			//fprintf(stderr,"2 %s: %d \r\n", __FUNCTION__, __LINE__);

		}else{//get queue empty
			pid = getpid();
			status = fut_enqueue(pid, f->set_queue);
			if(status == (pid32)SYSERR){
                                restore(mask);
                                return SYSERR;
                        }
			suspend(pid);
			f->value=value;
		}
		restore(mask);
                return OK;
	}	
	restore(mask);
	return SYSERR;
}
