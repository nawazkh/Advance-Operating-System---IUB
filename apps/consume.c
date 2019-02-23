#include <xinu.h>
#include <prodcons.h>

void consumer(int count) {
	while(1){
		//sleep(1);
		wait(produced);
		wait(mutex);
		printf("consumed value : %d \n", n);
		signal(mutex);
		if(n == count){
			break;
		}
		signal(consumed);
	}
	printf("Consumer ended.\n");
	printf("Deleting semaphores.\n");
	semdelete(produced);	
	semdelete(consumed);
	printf("Semaphores deleted.\n");
}
