/*
* future_get
* created by Nawaz
*/
#include <future.h>

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
	pid32 p;
		
	if(f->state == FUTURE_EMPTY){
		f->state = FUTURE_WAITING;
		//fprintf(stderr,"in future get called\n");
		if(f->flags == FUTURE_EXCLUSIVE){
			p = currpid;
			f->pid = p;
			//*value = *f->value;
			suspend(p);
			//fprintf(stderr,"released from suspended state\n");
			*value = *f->value;
			f->state = FUTURE_EMPTY;
			restore(mask);
                        return OK;
		}
	}
	/*
	if(f->flags == FUTURE_EXCLUSIVE){
		if(f->state == FUTURE_EMPTY){
			p = getpid();
			f->pid = p;
			f->state = FUTURE_WAITING;
			suspend(p);
                	value = f->value;
			//f->state = FUTURE_WAITING;
               			//fprintf(stderr,"in future_get calling block suspended");
                	restore(mask);
                	return OK;
        	}
		else if(f->state == FUTURE_FREE){
			*value = *f->value;
			f->state = FUTURE_EMPTY;
			restore(mask);
                        return OK;
		}
	}
	*/
	restore(mask);
	return SYSERR;
}
