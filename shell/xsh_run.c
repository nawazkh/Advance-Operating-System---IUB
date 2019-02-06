#include <xinu.h>
#include <prodcons.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//int n;                 //Definition for global variable 'n'
/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

shellcmd xsh_run(int nargs, char *args[]){
	/*int count = 20; // minimum values to be sent to prodcons*/
		int32	i;
	char	*argv[2];		/* argument vector for call	*/
	char	*src, *cmp;		/* used for string compare	*/
	int32	len;			/* length of a command name	*/
	int32	maxlen;			/* maximum length of all	*/
					/*   command names		*/
	int32	cols;			/* number of columns in the	*/
					/*   formatted command list	*/
	int32	spac;			/* space per column in the	*/
					/*   formatted command list	*/
	int32	lines;			/* total lines of output in the	*/
					/*   formatted command list	*/
	int32	j;			/* index of commands across one	*/
					/*   line of formatted output	*/
	
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("run, assignment 2.5; usage:\n run prodcons number");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}
	if (nargs > 3 || nargs < 3) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}
	
	if(strncmp(args[1],"prodcons",9) != 0){
		fprintf(stderr, "%s: argument in error\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}
	
	
	/*count = atoi(args[2]);*/
	if (nargs == 3) {
		for (i = 0; i < ncmd; i++) {
			src = cmdtab[i].cname;
			cmp = args[1];
			while (*src != NULLCH) {
				if (*src != *cmp) {
					break;
				}
				src++;
				cmp++;
			}
			if ( (*src != NULLCH) || (*cmp != NULLCH) ) {
				continue;
			}

			/* prepare args for shell command */

			argv[0] = args[1];
			argv[1] = args[2];
			(*cmdtab[i].cfunc) (2, argv);
			return 0;
		}
		printf("%s: no such command as '%s'\n", args[0], args[1]);
		return 1;
	}

	/*
	nargs--;	
	args[0] = args[1];
	args[1] = args[2];
	
	prodcons(nargs, args);*/
	return (0);
}
