#include <xinu.h>
#include <prodcons.h>

void producer(int count) {
	while(n < count){
		//sleep(1);
		wait(consumed);
		wait(mutex);
		n++;
		printf("Produced value : %d \n", n);
		signal(mutex);
		signal(produced);
	}
	printf("Producer ended.\n");
}
