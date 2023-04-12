#include "nw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
  int fd, cli, len;
  char buf[256];
  const char *towrite = "test message from server";

  fd = nws_tcp("0.0.0.0", "1337", 1);
  if (fd < 0) {
    fprintf(stderr, "error\n");
    exit(1);
  }

  cli = accept(fd, NULL, NULL);

  len = read(cli, buf, 256);

  printf("got:\n%.*s\n", len, buf);

  write(cli, towrite, strlen(towrite));
}
