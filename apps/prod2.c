#include <xinu.h>
#include <prodcons.h>
#include <tscdf-input1.h>

void prod2(int work_queue){
	printf("In producer");
	int i = 0;
	char *a;
	int st, ts, v;
		while(i < n_input){
			a = (char *)stream_input[i];
  			st = atoi(a);
			wait(arr_empty[st]);
			wait(arr_muttex[st]);
			if(arr_val[st].head >= work_queue){
				arr_val[st].head = 0;
			}
			// produce values and place it in the specific queue for consumer to consume
  			while (*a++ != '\t');
  			ts = atoi(a);
			buffers[st][arr_val[st].head].timestamp = ts;
  			while (*a++ != '\t');
  			v = atoi(a);
			buffers[st][arr_val[st].head].data = v;
			
			printf("\n stream to select %d, time stamp is %d, value is %d \n", st, ts, v);
			arr_val[st].head++;
			
			signal(arr_muttex[st]);
			signal(arr_full[st]);
			
			i++;
		}
		printf("producer complete");
}
