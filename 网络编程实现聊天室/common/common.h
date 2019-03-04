#ifndef _COMMON_H_
#define _COMMON_H_

#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
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
#include <pthread.h>

#ifdef DEBUG
	#define DBG(...) printf(__VA_ARGS__)
#else
	#define DBG(...)
#endif

struct Message {
	char from[20];
	int flag;
	char message[256];
	Message() {
		strcpy(from, "");
		strcpy(message, "");
		flag = 0;
	}
};

extern pthread_mutex_t mid;

//服务端父进程、子进程面对强制结束后的处理方法
void out(int sig);

//客户端父进程、子进程面对强制结束后的处理方法
void logout(int sig);

//将buffer内容写入Log_file文件中
int write_log(Message *buffer, char *Log_file);

//用于服务端的初始化
int socket_creat(int port);

//用于客户端的初始化
int socket_connect(int port, char *host);

//用于获取配置文件中的信息
int get_conf_val(char *filename, const char *key_name, char *val);

#endif
