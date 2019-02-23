/*************************************************************************
	> File Name: p.cpp
	> Author: Zcy
	> Mail: 296763002@qq.com
	> Created Time: 三  1/23 18:16:17 2019
 ************************************************************************/

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>    
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>

int main(int argc, char const *argv[]) {
	if(argc != 3) {
		printf("%s faild\n", argv[0]);
		exit(1);
	}
	int pd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in addr;
 	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	int port = atoi(argv[2]);
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	if (connect(pd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
		printf("connect faild\n");
		exit(1);
	}
	char *s = (char *)malloc(sizeof(char) * 1024);
	scanf("%[^\n]s", s);
	getchar();
	send(pd, s, 1024, 0);
	while(strcmp(s, "quit") != 0) {
		scanf("%[^\n]s", s);
		getchar();
		send(pd, s, 1024, 0);
	}
	free(s);
    close(pd);
	return 0;
}