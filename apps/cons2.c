#include <xinu.h>
#include <prodcons.h>
#include "tscdf/tscdf.h"

void cons2(int sid, int work_queue, int time_window, int output_time){
	printf("In consumer %d\n", sid);
	struct tscdf *my_tscdf  = tscdf_init(time_window); 
	int32 counter = 0, b=0;
	int32 *qarray;
	while(1){
		wait(arr_full[sid]);
		wait(arr_muttex[sid]);
		if(arr_val[sid].tail >= work_queue){
			arr_val[sid].tail = 0;
		}
		
		// consuming the data from the global queue dedication queue
		printf("hello from consumer %d, consumed values: timestamp:%d, value:%d \n",
                              sid, buffers[sid][arr_val[sid].tail].timestamp, buffers[sid][arr_val[sid].tail].data);
		tscdf_update(my_tscdf, buffers[sid][arr_val[sid].tail].timestamp, buffers[sid][arr_val[sid].tail].data);
		counter++;
		if(counter == output_time){
			qarray = tscdf_quartiles(my_tscdf);

			if(qarray == NULL) {
    				kprintf("tscdf_quartiles returned NULL\n");
			}else{
				for(b=0; b < 5; b++) {
   					kprintf("%d ", qarray[b]);
				}
				kprintf("\n");
      			}
			freemem((char *)qarray, (6*sizeof(int32)));
			counter = 0;
		}
		arr_val[sid].tail++;
	
		signal(arr_muttex[sid]);
		signal(arr_empty[sid]);
	}
	printf("Consumer ended.\n");
        printf("Deleting semaphores.\n");
	semdelete(arr_full[sid]);
        semdelete(arr_muttex[sid]);
        semdelete(arr_empty[sid]);
        printf("Semaphores deleted.\n");
}
