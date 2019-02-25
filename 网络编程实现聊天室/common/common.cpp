#include "./common.h"

//父进程获取到子进程结束后的处理方法
void handle(int sig) {
    if (sig == SIGCHLD) {
        exit(0);
    }
}

//将buffer内容写入Log_file文件中
int write_log(Message *buffer, char *Log_file) {
	FILE *file = NULL;
	if ((file = fopen(Log_file, "a")) == NULL) {
		perror("fopen faild\n");
		return -1;
	}
	if (fprintf(file, "%s: %s\n", buffer -> from, buffer -> message) < 0) {
		perror("fprintf faild");
		return -1;
	}
	fclose(file);
	return 0;
}


//服务端初始化
int socket_creat(int port) {
	int pd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pd == -1) {
		perror("socket faild");
		return -1;
	}
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(pd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
		perror("bind faild");
		close(pd);
		return -1;
	}
	if (listen(pd, 100) == -1) {
		perror("listen faild");
		close(pd);
		return -1;
	}
	return pd;
}

//客户端初始化
int socket_connect(int port, char *host) {
	int pd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in addr;
 	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host);
	if (connect(pd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
		perror("connect faild");
		close(pd);
		return -1;
	}
	return pd;
}

//用户获取配置文件信息
int get_conf_val(char *filename, char *key_name, char *val) {
	char *buffer = NULL;
	char *p = NULL;
	FILE *file = NULL;
	ssize_t len = 0;
	size_t chang = 0;
	if ((file = fopen(filename, "r")) == NULL) {
		perror("fopen faild\n");
		return -1;
	}
	while((len = getline(&buffer, &chang, file)) != -1) {
		p = strtok(buffer, "=");
		if (strcmp(key_name, p) == 0) {
			p = strtok(NULL, "\n");
			strcpy(val, p);
		}
		while(p != NULL) {
			p = strtok(NULL, "\n");
		}
	}
	fclose(file);
	if (val == NULL) {
		printf("conf_file_value faild\n");
		return -1;
	}
	return 0;
}