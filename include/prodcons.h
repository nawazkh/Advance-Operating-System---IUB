#include <xinu.h>   
#include <stddef.h>
#include <stdio.h> 
#include <future.h>

typedef struct Streamdata{
	int32 timestamp;
	int32 data;
} Sdata;

typedef struct StreamFuturedata{
        future* f_ts;
        future* f_d;
} SFdata;

typedef struct Queueval{
	int32 head;
	int32 tail;
} Qval;

/*Global variable for producer consumer*/
extern int n; /*this is just declaration*/

extern sid32 consumed, produced, mutex; 
extern sid32 *consr;

/* for multiple consumers and single producer */
extern int *arr_muttex, *arr_empty, *arr_full;
extern Qval *arr_val;
extern Sdata **buffers;
extern SFdata **f_buffers;

/*function Prototype*/
void consumer(int count);
void producer(int count);

void prod2(int work_queue);
void cons2(int sid, int work_queue, int time_window, int output_time);

