#include <xinu.h>
#include <future.h>

uint future_prod(future* fut,int value) {
	printf("Produced %d\n",value);
	future_set(fut,(char *)&value);
	return OK;
}

