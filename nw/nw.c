#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int nwc_tcp(const char *host, const char *port) {
	int sock, ret;
	struct addrinfo hints = {0}, *res, *p;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (host == NULL) {
		return -1;
	}

	ret = getaddrinfo(host, port, &hints, &res);
	if (ret != 0) {
		return ret;
	}

	for (p = res; p != NULL; p = p->ai_next) {
		sock = socket(p->ai_family, p->ai_socktype,
					  p->ai_protocol);
		if (sock == -1) {
			continue;
		}

		ret = connect(sock, p->ai_addr, p->ai_addrlen);
		if (ret == 0) {
			break;
		}
		
		close(sock);
	}

	freeaddrinfo(res);

	return sock;
}

int nws_tcp(const char *host, const char *port, int backlog) {
	int sock, ret;
	struct addrinfo hints = {0}, *res, *p;
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (host == NULL) {
		hints.ai_flags = AI_PASSIVE;
	}

	ret = getaddrinfo(host, port, &hints, &res);
	if (ret != 0) {
		return ret;
	}

	for (p = res; p != NULL; p = p->ai_next) {
		sock = socket(p->ai_family, p->ai_socktype,
					  p->ai_protocol);
		if (sock == -1) {
			continue;
		}
		ret = bind(sock, p->ai_addr, p->ai_addrlen);
		if (ret == 0) {
			break;
		}
		
		close(sock);
	}

	freeaddrinfo(res);

	if (p == NULL) {
		return -1;
	}

	ret = listen(sock, 5);
	if (ret == -1) {
		close(sock);
		return -1;
	}

	return sock;
}

int nwc_unix(const char *path, size_t len) {
	int sock, ret;
	struct sockaddr_un name = {0};

	sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (sock == -1) {
		return -1;
	}
	
	name.sun_family = AF_UNIX;
	memcpy(name.sun_path, path, len);
	ret = connect(sock, (struct sockaddr*)&name, sizeof(name));
	if (ret == -1) {
		close(sock);
		return -1;
	}

	return sock;
}

int nws_unix(const char *path, size_t len, int backlog) {
	int sock, ret;
	struct sockaddr_un name = {0};

	sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (sock == -1) {
		return -1;
	}

	name.sun_family = AF_UNIX;
	memcpy(name.sun_path, path, len);
	ret = bind(sock, (struct sockaddr*)&name, sizeof(name));
	if (ret == -1) {
		close(sock);
		return -1;
	}
	
	ret = listen(sock, backlog);
	if (ret == -1) {
		close(sock);
		return -1;
	}

	return sock;
}
