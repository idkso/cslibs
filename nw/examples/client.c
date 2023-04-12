#include "nw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  int fd, len;
  char buf[256];
  const char *towrite = "test message from client";

  fd = nwc_tcp("localhost", "1337");
  if (fd < 0) {
    fprintf(stderr, "error\n");
    exit(1);
  }

  write(fd, towrite, strlen(towrite));

  len = read(fd, buf, 256);

  printf("got:\n%.*s\n", len, buf);
}
