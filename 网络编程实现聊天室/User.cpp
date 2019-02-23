/*************************************************************************
	> File Name: p.cpp
	> Author: Zcy
	> Mail: 296763002@qq.com
	> Created Time: 三  1/23 18:16:17 2019
 ************************************************************************/

#include "./common/common.h"

int main(int argc, char const *argv[]) {
	if(argc != 3) {
		printf("%s faild\n", argv[0]);
		exit(1);
	}
	char host[105];
	strcpy(host, argv[1]);
	int port = atoi(argv[2]);
	int pd = socket_connect(port, host);
	if (pd == -1) {
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