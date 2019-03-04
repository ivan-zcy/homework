#include "./common/common.h"

//互斥信号量
pthread_mutex_t mid;

int main() {
	//取得配置信息路径
	char conf_file_client[20] = "./User.conf";

	//获取配置文件的相关信息
	char host[20] = "";
	char s[256] = "";
	char port_string[10] = "";
	char name[20] = "";
	char Log_file[105] = "";
	if (get_conf_val(conf_file_client, "Log_File", Log_file) == -1) {
		exit(1);
	}
	if (get_conf_val(conf_file_client, "Server_Ip", host) == -1) {
		exit(1);
	}
	if (get_conf_val(conf_file_client, "Server_Port", port_string) == -1) {
		exit(1);
	}
    if (get_conf_val(conf_file_client, "My_Name", name) == -1) {
		exit(1);
	}
	int port = atoi(port_string);

	//初始化客户端
	int pd = socket_connect(port, host);
	if (pd == -1) {
		exit(1);
	}

	//发送姓名信息
	send(pd, name, strlen(name) + 1, 0);

	//创建子进程，父进程用于接收服务端信息，子进程用于发送信息
	int pid = fork();
	if (pid < 0) {
		perror("fork faild");
		exit(1);
	}

	//初始化信号量
	pthread_mutex_init(&mid, NULL);

	//子进程
	if (!pid) {
		//面对ctrl+c采用out函数处理办法
		signal(SIGINT, out);

		//循环发送内容
		while(1) {
			printf("请输入内容:");
			scanf("%[^\n]s", s);
			getchar();
			if (send(pd, s, strlen(s) + 1, 0) == -1) {
				perror("send faild");
				break;
			}
		}

		//关闭并退出进程
		pthread_mutex_destroy(&mid);
		close(pd);
		exit(0);
	}
	//父进程 
	else {
		//面对ctrl+c采用logout函数处理办法
		signal(SIGINT, logout);

		//循环接收服务端的所有信息
		while(1) {
			Message buffer;
			int recv_len = recv(pd, &buffer, sizeof(Message), 0);
			if (recv_len == -1 || recv_len == 0) {
				break;
			}

			//将获取的信息写入到文件
			recv_len = write_log(&buffer, Log_file);
			if (recv_len == -1) {
				break;
			}
		}

		//强制关闭子进程并关闭父进程
		pthread_mutex_destroy(&mid);
		kill(pid, SIGKILL);
		close(pd);
		exit(0);
	}
	return 0;
}
