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

int main() {
	int pd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pd == -1) {
		printf("socket faild\n");
		exit(1);
	}
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(52013);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(pd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
		printf("bind faild\n");
		exit(1);
	}
	if (listen(pd, 10) == -1) {
		printf("listen faild\n");
		exit(1);
	}
	struct sockaddr_in addr_user;
	bzero(&addr_user, sizeof(addr_user));
	while(true) {
		int len = sizeof(addr_user);
		int newpd = accept(pd, (struct sockaddr *)&addr_user, (socklen_t *)&len);
		if (newpd == -1) {
			printf("accept faild\n");
			continue;
		}
		int pid = fork();
		if (!pid) {
			char *name = (char *)malloc(sizeof(char) * 1024);
			char *buffer = (char *)malloc(sizeof(char) * 1024);
			int recv_len = recv(newpd, name, 1024, 0);
			if (recv_len == -1) {
				free(name);
				free(buffer);
				close(newpd);
				exit(0);
			}
			name[recv_len] = '\0';
			printf("用户%s,ip为:%s从%d端口进入聊天室\n", name, inet_ntoa(addr_user.sin_addr), htons(addr_user.sin_port));
			while(1) {
				printf("%s:", name);
				recv_len = recv(newpd, buffer, 1024, 0);
				if (recv_len == -1 || recv_len == 0) {
					break;
				}
				buffer[recv_len] = '\0';
				printf("%s\n", buffer);
			}
			printf("用户%s退出聊天室\n", name);
			free(name);
			free(buffer);
			close(newpd);
			exit(0);
		} else {
			continue;
		}
	}
	close(pd);
	return 0;
}