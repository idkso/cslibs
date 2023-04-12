#ifndef NW_LIB_H
#define NW_LIB_H
#include <stdint.h>

/* returns socket fd that's connected to tcp socket at `path`:`port` */
int nwc_tcp(const char *host, const char *port);

/* returns socket fd that's listening at `path`:`port`
   `backlog` is self explanatory */
int nws_tcp(const char *host, const char *port, int backlog);

/* returns socket fd that's connected to unix socket at `path`
   `len` is the length of `path` */
int nwc_unix(const char *path, size_t len);

/* returns socket fd that's listening at `path`
   `len` is the length of `path`
   `backlog` is self explanatory */
int nws_unix(const char *path, size_t len, int backlog);
#endif
