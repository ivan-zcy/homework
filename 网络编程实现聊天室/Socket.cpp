/*************************************************************************
	> File Name: p.cpp
	> Author: Zcy
	> Mail: 296763002@qq.com
	> Created Time: 三  1/23 18:16:17 2019
 ************************************************************************/

#include "./list/list.h"

int main() {
	//取得配置信息路径
	char conf_file_socket[20] = "./Server.conf";

	//子进程或父进程接收到ctrl+c执行自定义函数
	signal(SIGINT, out);
	
	//获取相关配置文件信息
	char port_string_client[10]="";
	char port_string_socket[10]="";
	if (get_conf_val(conf_file_socket, "Server_Port", port_string_socket) == -1) {
		exit(1);
	}
	if (get_conf_val(conf_file_socket, "Client_Port", port_string_client) == -1) {
		exit(1);
	}
	int Server_Port = atoi(port_string_socket);
	int Client_Port = atoi(port_string_client);

	//初始化服务端和用户存储链表
	int pd = socket_creat(Server_Port);
	if (pd == -1) {
		exit(1);
	}
	struct sockaddr_in addr_user;
	bzero(&addr_user, sizeof(addr_user));
	LIST list;
	init(&list);

	//循环接收外界数据
	while(true) {
		int len = sizeof(addr_user);
		int newpd = accept(pd, (struct sockaddr *)&addr_user, (socklen_t *)&len);
		if (newpd == -1) {
			perror("accept faild");
			continue;
		}

		//若接收到，则创建一个子进程，循环接收它的所有内容
		int pid = fork();
		if (pid < 0) {
			perror("fork faild");
			continue;
		}
		if (!pid) {
			
			//关闭pd并初始化消息和用户结构体以及缓冲区
			close(pd);
			USER user;
			Message message;
			char name[20];
			char buffer[300];

			//接收对方传来的用户名
			int recv_len = recv(newpd, name, 20, 0);
			if (recv_len == -1) {
				close(newpd);
				exit(1);
			}
			strcpy(message.from, name);

			//创建用户信息并加入到链表
			strcpy(user.name, name);
			strcpy(user.host, inet_ntoa(addr_user.sin_addr));
			user.port = Client_Port;
			user.next = NULL;
			add(&list, &user);

			//服务端输出登陆信息
			printf("用户\033[;31m%s\033[0m,ip为:\033[;32m%s\033[0m从\033[;33m%d\033[0m端口进入聊天室\n", name, inet_ntoa(addr_user.sin_addr), htons(addr_user.sin_port));

			//服务器向用户输出欢迎信息
			message.flag = 2;
			sprintf(message.message, "%s，欢迎登陆聊天室", name);
			query(&list, name, &message);

			//服务器向除该用户以外的用户输出提示信息
			sprintf(message.message, "您的聊天室好友%s上线啦，快打声招呼吧", name);
			query_other(&list, name, &message);

			//循环接收该用户发来的消息
			while(1) {

				recv_len = recv(newpd, buffer, 256, 0);
				if (recv_len == -1 || recv_len == 0) {
					break;
				}
				DBG("recv_len:%d\n", recv_len);

				//不同的输入执行服务器执行不同的操作
				if (strcmp(buffer, "#") == 0) {
					char s[256];
					query_now(&list, s);
					message.flag = 2;
					strcpy(message.message, s);
					query(&list, name, &message);
				} else if (buffer[0] == '@') {
					char buffer_copy[256];
					char goal_name[20];
					char *p = NULL;
					message.flag = 1;
					strcpy(buffer_copy, buffer);
					p = strtok(buffer_copy, " ");
					strcpy(goal_name, p + 1);
					p = strtok(NULL, "\n");
					strcpy(message.message, p);
					if (query(&list, goal_name, &message) == -1) {
						message.flag = 2;
						sprintf(message.message, "您的好友%s已下线", goal_name);
						query(&list, name, &message);
					}
				} else {
					message.flag = 0;
					strcpy(message.message, buffer);
					getout(&list, &message);
				}
				printf("\033[;31m%s:\033[0m %s\n", name, buffer);
				fflush(stdout);
			}

			//断开链接后，在链表中删除该用户
			del(&list, name);

			//在服务端显示用户退出
			printf("\033[;33m用户%s退出聊天室\n\033[0m", name);

			//通知其他在线用户该用户已下线
			message.flag = 2;
			sprintf(message.message, "用户%s退出聊天室啦", name);
			getout(&list, &message);
			close(newpd);
			exit(0);
		}
		close(newpd);
	}
	close(pd);
	return 0;
}