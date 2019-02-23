#include "./common.h"

int socket_creat(int port) {
	int pd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pd == -1) {
		perror("socket faild");
		return -1;
	}
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(52013);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(pd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
		perror("bind faild");
		close(pd);
		return -1;
	}
	if (listen(pd, 10) == -1) {
		perror("listen faild");
		close(pd);
		return -1;
	}
	return pd;
}

int socket_connect(int port, char *host) {
	int pd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in addr;
 	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host);
	if (connect(pd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
		perror("connect faild");
		return -1;
	}
	return pd;
}