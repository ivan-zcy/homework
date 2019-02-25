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

#ifdef DEBUG
	#define DBG(...) printf(__VA_ARGS__)
#else
	#define DBG(...)
#endif

struct Message {
	char from[20];
	int flag;
	char message[1024];
};

//父进程获取到子进程结束后的处理方法
void handle(int sig);

//将buffer内容写入Log_file文件中
int write_log(Message *buffer, char *Log_file);

//用于服务端的初始化
int socket_creat(int port);

//用于客户端的初始化
int socket_connect(int port, char *host);

//用于获取配置文件中的信息
int get_conf_val(char *filename, char *key_name, char *val);
