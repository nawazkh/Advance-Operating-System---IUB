#include <xinu.h>
#include <future.h>
#include <stdio.h>
#include <stdlib.h>

uint32 future_ring(future *in, future *out);

/**
 * Test Futures
 */
shellcmd future_test(int nargs, char *args[])
{
  int ring = 0;
  int future_flags = 0;
  int ring_count = 10;
  int final_val;
  int i;

#ifndef NFUTURE
  printf("no support for futures (NFUTURES not defined.)\n");
  return OK;
#endif

  if (nargs == 2 && strncmp(args[1], "-r", 2) == 0) {
    ring = 1;
    printf("Producer/consumer process ring\n");
  }
  else {
    printf("No valid options\n");
    return(OK);
  }

  if (ring == 1) {
    future *first, *in, *out = NULL;
    first = future_alloc(future_flags, sizeof(int));
    in = first;
    for (i=0; i < ring_count; i++) {
      out = future_alloc(future_flags, sizeof(int));
      resume( create(future_ring, 1024, 20, i, 2, in, out) );
      in = out;
    }
    printf("master sets %d\n", ring_count);
    future_set(first, (char *)&ring_count);
    future_get(out, (char *)&final_val);
    printf("master gets %d\n", final_val);
    return(OK);
  }
  return(OK);
}

uint32 future_ring(future *in, future *out) {
  int val;
  //fprintf(stderr,"----in future ring----\n");
  future_get(in, (char *)&val);
  //fprintf(stderr,"----future get success----\n");
  printf("Process %d gets %d, puts %d\n", getpid(), val, val-1);
  val--;
  future_free(in);
  //fprintf(stderr,"future free success\n");
  future_set(out, (char *)&val);
  //fprintf(stderr,"future set after free success\n");
  return OK;
}
