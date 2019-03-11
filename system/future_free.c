/*
* future_free frees the allocated future.
* created by Nawaz
*/
#include <future.h>

/*---------------------
* future_free Frees the allocated future.
* pararms:
*	future_t* f: 		future to free
* return type:
*	syscall:                SYSERR orr OK
----------------------*/

syscall future_free(future* f){
	intmask mask;
	mask = disable();
	// de allocate the memory in the 
	freemem((char*)f->value, sizeof(char));
	freemem((char*)f, sizeof(f));
	restore(mask);
	return OK;
}
