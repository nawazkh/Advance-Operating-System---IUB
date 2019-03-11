/*
* future_set
* created by Nawaz
*/
#include <future.h>

/*---------------------
* future_set set the value of a future and may change the state of the future
* pararms:
*	future_t* f:    Future on which to wait for and obtain value
*	char* value:    Address into which the future’s value will be copied.
* return type:
*	syscall:                SYSERR orr OK
----------------------*/
syscall future_set(future* f, char* value){
	pid32 pid;
	intmask mask;
	mask = disable();
	fprintf(stderr,"in future set");
	if(f->state == FUTURE_EMPTY){
		if(f->flags == FUTURE_EXCLUSIVE){
			f->value = value;
			f->state = FUTURE_FULL;
			restore(mask);
                	return OK;
		}	
	}
	
	if(f->state == FUTURE_WAITING){
                if(f->flags == FUTURE_EXCLUSIVE){
			f->value = value;
                	f->state = FUTURE_EMPTY;
                	pid = f->pid;
                	resume(pid);
                	restore(mask);
                	return OK;
		}
	}
	
	/*
	if(f->flags == FUTURE_EXCLUSIVE){
		if(f->state == FUTURE_EMPTY){
				//fprintf(stderr,"in future Set of future_set\n");
			f->value = value;
				//fprintf(stderr,"in future set step 1 success\n");
			f->state = FUTURE_FULL;
				//fprintf(stderr,"in future set step 2 success\n");
			restore(mask);
			return OK;
		}
		else if(f->state == FUTURE_WAITING){
			f->value = value;
			f->state = FUTURE_EMPTY;
			pid = f->pid;
			resume(pid);
//			*f->value = *value;
//			f->state = FUTURE_EMPTY;
			restore(mask);
                        return OK;
		}
	}
	*/
	restore(mask);
	return SYSERR;
}
