/*
* future_alloc allocates a new future with the given mode. 
* created by Nawaz
*/
#include <future.h>

/*---------------------
* future_alloc allocates a new future with the given mode.
* pararms:
*	int future_flags:	The mode this future will operate in
*	uint size:		Size of the value object
* return type: 
* 	future_t:		NULL or a pointer to a valid future
*----------------------*/

future* future_alloc(int future_flags, uint size){
	future *f;
	intmask mask;
	mask = disable();
	if(future_flags == FUTURE_EXCLUSIVE){
			//  fprintf(stderr,"future flags yes\n");
		// get future some memory
		f = (future * )getmem(sizeof(future));
			//  fprintf(stderr,"1\n");	
		// value as per the size specified
		f->value = (char *)getmem(sizeof(char)*size);
			//  fprintf(stderr,"2\n");
		f->state = FUTURE_EMPTY;
		f->flags = FUTURE_EXCLUSIVE;
		f->size = size;
//		fprintf(stderr,"future allocated with following details\nprocess id: %d, f->value: %d, f->state: %d, f->flags:%d\n",getpid(),*f->value,f->state,f->flags);
	}
		//fprintf(stderr,"test val %d\n",f->state);
	restore(mask);
	return f;
}
