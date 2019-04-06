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
		
	f = (future * )getmem(sizeof(future));
	//fprintf(stderr,"Future Flags:%d\n",future_flags);		
	
	// value as per the size specified
	f->state = FUTURE_EMPTY;
	f->flags = future_flags;
	f->size = size;
	f->value = (char *)getmem(sizeof(char)*size);	

	if(future_flags != FUTURE_EXCLUSIVE){
		if(future_flags != FUTURE_SHARED){
			// create set queue
        		myq* setq = (myq*)getmem(sizeof(myq));
			//setq->total = 50;// having a queue of length 50
        		setq->front = 0;
        		setq->rear = -1;
        		setq->size=0;
			//setq->parray=(int*)getmem(setq->total * sizeof(int));
        		f->set_queue = setq;
		}
		// create get queue
        	myq* getq = (myq*)getmem(sizeof(myq));
        	getq->front = 0;
        	getq->rear = -1;
        	getq->size=0;
		//getq->total=50;
		//getq->parray=(int*)getmem(getq->total * sizeof(int));
        	f->get_queue = getq;
	}
	// consumer and producer counts for all the futures.
	f->consumer_count = 0;
	f->producer_count = 0;
	f->prodcon_shared_count = 0;	

	//fprintf(stderr,"future allocated with following details\nprocess id: %d, f->value: %d, f->state: %d, f->flags:%d\n",getpid(),*f->value,f->state,f->flags);

	restore(mask);
	return f;
}
