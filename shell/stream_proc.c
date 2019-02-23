#include <xinu.h>
#include <prodcons.h>
#include <stdio.h>
#include <stdlib.h>

int n;    //Definition for global variable 'n'
/* Now global variable n will be on Heap so 
it is accessible all the processes i.e. consume and produce */

int *arr_muttex, *arr_empty, *arr_full;
Qval *arr_val;
Sdata **buffers;

shellcmd stream_proc(int nargs, char *args[]){	
	char usage[] = "Usage: -s num_streams -w work_queue -t time_window -o output_time\n";
	
	int num_streams = 0, work_queue = 0, time_window = 0, output_time = 0;
        int i = 0, c;
        char *ch;

	printf("\n In stream proc\n");

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\t prodcons, assignment 3; usage:\n %s",usage);
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}
	
	if (!(nargs % 2)) {
  		printf("%s", usage);
  		return(-1);
	}
	else {
  		i = nargs - 1;
		while (i > 0) {
    			ch = args[i-1];
    			c = *(++ch);
    
    			switch(c) {
    				case 's':
      				num_streams = atoi(args[i]);
      				break;

    				case 'w':
      				work_queue = atoi(args[i]);
      				break;

    				case 't':
      				time_window = atoi(args[i]);
      				break;
      	
    				case 'o':
      				output_time = atoi(args[i]);
      				break;

				default:
      				printf("%s", usage);
      				return(-1);
    			}

    			i -= 2;
  		}
	}
	//num_streams = 0, work_queue = 0, time_window = 0, output_time = 0
	printf("num_streams:%d \n",num_streams);
	printf("work_queues:%d \n",work_queue);
	printf("time_window:%d \n",time_window);
	printf("output_time:%d \n",output_time);
	
	/* initlializing all the control values */
	/*
		int arr_muttex*, arr_empty*, arr_full*;	
		stream_data arr_val*;	
	*/
	if(num_streams == 0) return -1;
	
	arr_muttex = (int *)getmem(sizeof(int)*(num_streams));
	arr_empty = (int *)getmem(sizeof(int)*(num_streams));
	arr_full = (int *)getmem(sizeof(int)*(num_streams));
	arr_val = (Qval *)getmem(sizeof(Qval)*(num_streams));
	
	i = 0;
	while(i < num_streams){
		arr_muttex[i] = semcreate(1);
		arr_empty[i] = semcreate(work_queue);
		arr_full[i] = semcreate(0);
		arr_val[i].head = 0;
		arr_val[i].tail = 0;
		i++;
	}
	buffers = (Sdata **)getmem(sizeof(Sdata *)*(num_streams));
	i = 0;
	while(i < num_streams){
		Sdata *val = (Sdata *)getmem(sizeof(Sdata)*(work_queue));
		int j = 0;
		while(j < work_queue){
			val[j].timestamp = 0;
			val[j].data = 0;
			j++;
		}
		buffers[i] = val;
		i++;
	}
	
	printf("Successfully created the limiting values\n");
	
		
	/* creating consumer first. Consumer will halt at the first call until producer produces values  */
	int j = 0;
	while(j < num_streams){
		resume(create(cons2, 1024, 20, "cons2", 4, j, work_queue, time_window, output_time));
		j++;
	}
	
	
	/* Creating producer  */
	resume(create(prod2, 1024, 20, "prod2", 1, work_queue));
	
	printf("Producers and Consumers end \n");
	

	// creating one producer
	/* produced = semcreate(0);
        resume(create(prod2, 1024, 20, "prod2", 1, num_streams));
	
	mutex = semcreate(1);
	*/
	
	return 0;
}
