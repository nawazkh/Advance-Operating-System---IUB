#include <xinu.h>
#include <prodcons.h>

void cons2(int sid, int work_queue, int time_window, int output_time){
	printf("In consumer %d\n", sid);
	
	while(1){
		wait(arr_full[sid]);
		wait(arr_muttex[sid]);
		if(arr_val[sid].tail >= work_queue){
			arr_val[sid].tail = 0;
		}
		
		// consuming the data from the global queue dedication queue
		printf("hello from consumer %d, consumed values: timestamp:%d, value:%d \n",
                              sid, buffers[sid][arr_val[sid].tail].timestamp, buffers[sid][arr_val[sid].tail].data);
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
