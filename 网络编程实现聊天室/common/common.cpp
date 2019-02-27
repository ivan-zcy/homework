#include "./common.h"

//服务端父进程、子进程面对强制结束后的处理方法
void out(int sig) {
	_exit(0);
}

//客户端父进程、子进程面对强制结束后的处理方法
void logout(int sig) {
	//客户端配置文件以及获取相关信息
	char conf_file_client[20] = "./User.conf";
	char Log_file[105] = "";
	FILE *file = NULL;
	if(get_conf_val(conf_file_client, "Log_File", Log_file) == -1) {
		_exit(0);
	}

	//打开日志文件并写入退出信息
	if ((file = fopen(Log_file, "a+")) == NULL) {
		perror("fopen faild");
		_exit(0);
	}
	if (fprintf(file, "\033[;43m您已退出登陆\033[0m\n") < 0) {
		fclose(file);
		perror("fprintf faild");
		_exit(0);
	}

	//关闭文件并退出进程
	fclose(file);
	_exit(0);
}

//将buffer内容写入Log_file(日志)文件中
int write_log(Message *buffer, char *Log_file) {
	//打开日志文件
	FILE *file = NULL;
	if ((file = fopen(Log_file, "a+")) == NULL) {
		perror("fopen faild");
		return -1;
	}

	//根据不同的信息类型进行按照不同方式写入
	switch (buffer -> flag) {
		case 0:
			fprintf(file, "\033[;32m%s(@全体): \033[0m%s\n", buffer -> from, buffer -> message);
			break;
		case 1:
			fprintf(file, "\033[;31m%s(私聊你): \033[0m%s\n", buffer -> from, buffer -> message);
			break;
		default:
			fprintf(file, "\033[;33m(系统通知): %s\033[0m\n", buffer -> message);
	}

	//关闭文件
	fclose(file);
	return 0;
}


//服务端初始化并返回套接字
int socket_creat(int port) {
	//创建套接字
	int pd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pd == -1) {
		perror("socket faild");
		return -1;
	}

	//绑定端口
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

	//进行监听
	if (listen(pd, 100) == -1) {
		perror("listen faild");
		close(pd);
		return -1;
	}
	return pd;
}

//客户端初始化并返回套接字
int socket_connect(int port, char *host) {
	//创建套接字
	int pd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//建立连接
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
int get_conf_val(char *filename, const char *key_name, char *val) {
	//打开配置文件
	char *buffer = NULL;
	char *p = NULL;
	FILE *file = NULL;
	ssize_t len = 0;
	size_t chang = 0;
	if ((file = fopen(filename, "r")) == NULL) {
		perror("fopen faild");
		return -1;
	}

	//循环读入每行信息到buffer
	while((len = getline(&buffer, &chang, file)) != -1) {
		//截取=前后两部分，若前半部分==key_name，那么val值为后半部分
		p = strtok(buffer, "=");
		if (strcmp(key_name, p) == 0) {
			p = strtok(NULL, "\n");
			strcpy(val, p);
		}
		while(p != NULL) {
			p = strtok(NULL, "\n");
		}
	}

	//关闭文件并判断是否成功读取
	fclose(file);
	if (strcmp(val, "") == 0) {
		printf("conf_file_value faild\n");
		return -1;
	}
	return 0;
}