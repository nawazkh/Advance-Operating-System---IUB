#include <xinu.h>
#include <string.h>
#include <stdio.h>

shellcmd xsh_hello(int nargs, char *args[])
{
	int32	i;
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s \n\n", args[0]);
		printf("Description:\n");
		printf("\tUse hello command to display hello <string> on user console \n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}
	/* Check for valid number of arguments */

	if (nargs != 2) {
		fprintf(stderr, "%s: invalid arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}
	if(nargs > 1){
		printf("Hello %s, Welcome to the world of Xinu!!\n",args[1]);
	}
	return 0;
}
