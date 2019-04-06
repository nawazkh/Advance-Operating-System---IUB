#include <xinu.h>
#include <future.h>
#include <stdio.h>
#include <stdlib.h>

uint32 future_ring(future *in, future *out);
future **fibfut;
int zero = 0;
int one = 1;

int ffib(int);

/**
 * Test Futures
 */
shellcmd future_test(int nargs, char *args[])
{
	int ring = 0;
	int future_flags = 0;
	int ring_count = 10;
	char final_val;
	int i;
	int fib = -1;

	#ifndef NFUTURE
		printf("no support for futures (NFUTURES not defined.)\n");
  		return OK;
	#endif
	fprintf(stderr,"nargs: %d\n",nargs);
  	if (nargs == 2 && strncmp(args[1], "-r", 2) == 0) {
    		ring = 1;
    		printf("Producer/consumer process ring\n");
  	}
	else if (nargs == 2 && strncmp(args[1], "-pc", 2) == 0) {
                ring = 2;
                printf("Queue producer/consumer process ring\n");
        }
  	else if(nargs == 3 && strncmp(args[1], "-r", 2) == 0){
    		//fprintf(stderr,"args[0]: %d, args[1]: %d, args[2]: %d\n",args[0],args[1],args[2]);
    		if(checkNum(args[2])){
      			ring = 1;
      			ring_count = atoi(args[2]);
    		}else{
      			printf("Invalid number. Please enter integer after future_test\n");
      			return (OK);
    		}
  	}
	else if(nargs == 3 && strncmp(args[1], "-f", 2) == 0){
                //fprintf(stderr,"args[0]: %d, args[1]: %d, args[2]: %d\n",arg$
                if(checkNum(args[2])){
                        ring = 3;
                        fib = atoi(args[2]);
                }else{
                      	printf("Invalid number. Please enter integer after future_test\n");
                        return (OK);
                }
        }
  	else {
    		printf("No valid options\n");
    		return(OK);
  	}

	if(ring == 3){
		if(fib > -1){
			future_flags = FUTURE_SHARED;
			//future* f_shared = future_alloc(future_flags, sizeof(int));
			int final_fib;    
			/* create the array of future pointers */
    			if ((fibfut = (future **)getmem(sizeof(future *) * (fib + 1))) == (future **)SYSERR) {
      				printf("getmem failed\n");
      				return(SYSERR);
    			}
    
    			/* get futures for the future array */
    			for (i=0; i <= fib; i++) {
      				if((fibfut[i] = future_alloc(future_flags, sizeof(int))) == (future *)SYSERR) {
        				printf("future_alloc failed\n");
        				return(SYSERR);
      				}
    			}
			
			for(i = 0; i <= fib; i++){
				resume( create(ffib, 1024, 20, "fib"+i, 1,i) );
			}
			//resume( create(ffib, 1024, 20, "fprod2", 1, 1) );
			future_get(fibfut[fib], (char *)&final_fib);

   			for (i=0; i <= fib; i++) {
      				future_free(fibfut[i]);
    			}

    			freemem((char *)fibfut, sizeof(future *) * (fib + 1));    
    			printf("Nth Fibonacci value for N=%d is %d\n", fib, final_fib);
    			return(OK);
			
		}else{
			printf("Please enter valid fibbonacci number \n");
		}
	}
	else if(ring == 2){
		fprintf(stderr,"Starting the queue based prodcons\n");
		future* f_exclusive,* f_shared,* f_queue = NULL;
		
		f_exclusive = future_alloc(FUTURE_EXCLUSIVE, sizeof(int));
  		f_shared    = future_alloc(FUTURE_SHARED, sizeof(int));
  		f_queue     = future_alloc(FUTURE_QUEUE, sizeof(int));
		
		// Test FUTURE_EXCLUSIVE
		resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
  		resume( create(future_prod, 1024, 20, "fprod1", 2, f_exclusive, 1) );

  		// Test FUTURE_SHARED
  		resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
  		resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
  		resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared) ); 
  		resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
  		resume( create(future_prod, 1024, 20, "fprod2", 2, f_shared, 2) );

  		// Test FUTURE_QUEUE
  		resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
  		resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
 		resume( create(future_cons, 1024, 20, "fcons8", 1, f_queue) );
  		resume( create(future_cons, 1024, 20, "fcons9", 1, f_queue) );
  		resume( create(future_prod, 1024, 20, "fprod3", 2, f_queue, 3) );
  		resume( create(future_prod, 1024, 20, "fprod4", 2, f_queue, 4) );
  		resume( create(future_prod, 1024, 20, "fprod5", 2, f_queue, 5) );
  		resume( create(future_prod, 1024, 20, "fprod6", 2, f_queue, 6) );

	}
  	else if (ring == 1) {
    		future *first, *in, *out = NULL;
    		first = future_alloc(future_flags, sizeof(int));
    		in = first;
    		for (i=0; i < ring_count; i++) {
      			out = future_alloc(future_flags, sizeof(int));
      			resume( create(future_ring, 1024, 20, "", 2, in, out) );
      			in = out;
    		}
    		printf("master sets %d\n", ring_count);
    		future_set(first, (char *)&ring_count);
    		future_get(out, (char *)&final_val);
    		printf("master gets %d\n", final_val);
    		return(OK);
  	}
  	return(OK);
}

uint32 future_ring(future *in, future *out) {
	int val;
	//fprintf(stderr,"----in future ring----\n");
  	future_get(in, (char *)&val);
  	//fprintf(stderr,"----future get success----\n");
  	printf("Process %d gets %d, puts %d\n", getpid(), val, val-1);
  	val--;
  	future_free(in);
  	//fprintf(stderr,"future free success\n");
  	future_set(out, (char *)&val);
  	//fprintf(stderr,"future set after free success\n");
  	return OK;
}

int ffib(int n) {

        int minus1 = 0;
        int minus2 = 0;
        int this = 0;

        if (n == 0) {
                future_set(fibfut[0],(char *)&zero);
                return OK;
        }

	if (n == 1) {
                future_set(fibfut[1],(char *)&one);
                return OK;
        }
	fprintf(stderr,"----in fibb----\n");
	fprintf(stderr,"n is %d\n",n);
	future_get(fibfut[n-2],(char *)&minus2);
        future_get(fibfut[n-1],(char *)&minus1);
	fprintf(stderr,"minus1 is %d,minus2 is%d\n",minus1,minus2);
        this = minus1 + minus2;

        future_set(fibfut[n], (char *)&this);

        return(0);

}
