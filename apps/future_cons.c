#include <xinu.h>
#include <future.h>

uint future_cons(future* fut) {
  char i;
  int status;
  status = (int)future_get(fut,(char *)&i);
  if (status < 1) {
    printf("future_get failed\n");
    return -1;
  }
  printf("Consumed %d\n",i);
  return OK;
}
