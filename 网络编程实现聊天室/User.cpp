#include "./common/common.h"

int main() {
	//取得配置信息路径
	char conf_file_client[20] = "./User.conf";

	//创建子进程，父进程用于接收服务端信息，子进程用于发送信息
	int pid = fork();
	if (pid < 0) {
		perror("fork faild");
		exit(1);
	}

	//子进程
	if (!pid) {
		//面对ctrl+c采用out函数处理办法
		signal(SIGINT, out);

		//获取配置文件的相关信息
		char host[20] = "";
		char s[256] = "";
		char port_string[10] = "";
		char name[20] = "";
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
		send(pd, name, 20, 0);

		//循环发送内容
		while(1) {
			scanf("%[^\n]s", s);
			getchar();
			send(pd, s, 256, 0);
		}

		//关闭并退出进程
		close(pd);
		exit(0);
	}
	//父进程 
	else {
		//面对ctrl+c采用logout函数处理办法
		signal(SIGINT, logout);
		
		//获取配置文件的相关信息
		char port_string[10] = "";
		char Log_file[105] = "";
		if (get_conf_val(conf_file_client, "Clinet_Port", port_string) == -1) {
			kill(pid, SIGKILL);
			exit(1);
		}
		if (get_conf_val(conf_file_client, "Log_File", Log_file) == -1) {
			kill(pid, SIGKILL);
			exit(1);
		}
		int port = atoi(port_string);

		//初始化服务端
		int pd = socket_creat(port);
		if (pd == -1) {
			kill(pid, SIGKILL);
			exit(1);
		}
		struct sockaddr_in addr_user;
		bzero(&addr_user, sizeof(addr_user));

		//循环接收信息
		while(true) {
			int len = sizeof(addr_user);
			int newpd = accept(pd, (struct sockaddr *)&addr_user, (socklen_t *)&len);
			if (newpd == -1) {
				perror("accept faild");
				continue;
			}

			//接收到外界信号，创建一个子进程用于循环接收该外界的所有信息
			int pid_son = fork();
			if (pid_son < 0) {
				perror("fork faild");
				continue;
			}
			if (!pid_son) {
				close(pd);
				while(1) {
					Message buffer;
					int recv_len = recv(newpd, &buffer, sizeof(Message), 0);
					if (recv_len == -1 || recv_len == 0) {
						break;
					}
					//将获取的信息写入到文件
					recv_len = write_log(&buffer, Log_file);
					if (recv_len == -1) {
						close(newpd);
						exit(1);
					}
				}
				close(newpd);
				exit(0);
			}
			close(newpd);
		}
		
		//强制关闭子进程并关闭父进程
		kill(pid, SIGKILL);
		close(pd);
		exit(0);
	}
	return 0;
}