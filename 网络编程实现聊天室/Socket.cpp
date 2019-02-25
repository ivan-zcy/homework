/*************************************************************************
	> File Name: p.cpp
	> Author: Zcy
	> Mail: 296763002@qq.com
	> Created Time: 三  1/23 18:16:17 2019
 ************************************************************************/

#include "./common/common.h"

int main() {
	int pd = socket_creat(8731);
	if (pd == -1) {
		exit(1);
	}
	struct sockaddr_in addr_user;
	bzero(&addr_user, sizeof(addr_user));
	while(true) {
		int len = sizeof(addr_user);
		int newpd = accept(pd, (struct sockaddr *)&addr_user, (socklen_t *)&len);
		if (newpd == -1) {
			perror("accept faild");
			continue;
		}
		int pid = fork();
		if (pid < 0) {
			perror("fork faild");
			continue;
		}
		if (!pid) {
			close(pd);
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
			printf("用户\033[;31m%s\033[0m,ip为:\033[;32m%s\033[0m从\033[;33m%d\033[0m端口进入聊天室\n", name, inet_ntoa(addr_user.sin_addr), htons(addr_user.sin_port));
			while(1) {
				recv_len = recv(newpd, buffer, 1024, 0);
				if (recv_len == -1 || recv_len == 0) {
					break;
				}
				buffer[recv_len] = '\0';
				printf("\033[;31m%s:\033[0m", name);
				printf("%s\n", buffer);
				fflush(stdout);
			}
			printf("\033[;33m用户%s退出聊天室\n\033[0m", name);
			free(name);
			free(buffer);
			close(newpd);
			exit(0);
		}
		close(newpd);
	}
	close(pd);
	return 0;
}