#include <xinu.h>
#include <prodcons.h>
#include <stdlib.h>

int n;                 //Definition for global variable 'n'
/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

int checkNum(const char *str) 
{
    while(*str != '\0')
    {
        if(*str < '0' || *str > '9')
            return 0;
        str++;
    }
    return 1;
}

shellcmd xsh_prodcons(int nargs, char *args[])
{
  //Argument verifications and validations
	int count = 2000;             //local varible to hold count
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\prodcons, assignment 2; usage:\n prodcons number");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}
	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	if (nargs != 2) {
		fprintf(stderr, "%s: argument in error\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}
	if(!checkNum(args[1])){
		printf("Invalid number. Please enter integer after prodcons");
		return (0);
	}
	count = atoi(args[1]);
	//check args[1] if present assign value to count
    
	//create the process producer and consumer and put them in ready queue.
    	//Look at the definations of function create and resume in the system folder for reference.      
    	resume( create(producer, 1024, 20, "producer", 1, count));
    	resume( create(consumer, 1024, 20, "consumer", 1, count));
    	return (0);
  }
