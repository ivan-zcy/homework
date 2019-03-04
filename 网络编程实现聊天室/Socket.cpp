/*************************************************************************
	> File Name: p.cpp
	> Author: Zcy
	> Mail: 296763002@qq.com
	> Created Time: 三  1/23 18:16:17 2019
 ************************************************************************/

#include "./rwt/rwt.h"

//读者写者与5个信号量
int readCount, writeCount;
pthread_mutex_t rwt, RWmutex, mutex1, mutex2, mutex3;
//适配common.h
pthread_mutex_t mid;

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

	//初始化服务端和信号量
	int pd = socket_creat(Server_Port);
	if (pd == -1) {
		exit(1);
	}
	init();
	struct sockaddr_in addr_user;
	socklen_t socklen = sizeof(addr_user);
	bzero(&addr_user, socklen);


	//循环接收外界数据
	while(true) {
		//初始化链表
		LIST list;
		USER user;
		Message message;
		char name[25];

		//接收新的链接申请
		int newpd = accept(pd, (struct sockaddr *)&addr_user, &socklen);
		if (newpd == -1) {
			perror("accept faild");
			continue;
		}

		//接收对方传来的用户名
		int recv_len = recv(newpd, name, 20, 0);
    	if (recv_len == -1) {
			close(newpd);
			continue;
		}
		strcpy(message.from, name);

		//创建用户信息并存入文件
		strcpy(user.name, name);
		user.pd = newpd;
		user.next = NULL;

		//添加并更新用户链表文件
		if(reader(&list) == -1) {
			close(newpd);
			continue;
		}
		add(&list, &user);
		if(writer(&list) == -1) {
			close(newpd);
			continue;
		}

		//服务端输出登陆信息
		printf("用户\033[;31m%s\033[0m,ip为:\033[;32m%s\033[0m从\033[;33m%d\033[0m端口进入聊天室\n", name, inet_ntoa(addr_user.sin_addr), htons(addr_user.sin_port));

		//服务器向用户输出欢迎信息
		message.flag = 2;
		sprintf(message.message, "%s，欢迎登陆聊天室", name);
		query(&list, name, &message);

		//服务器向除该用户以外的用户输出提示信息
		sprintf(message.message, "您的聊天室好友%s上线啦，快打声招呼吧", name);
		query_other(&list, name, &message);

		//若接收到，则创建一个线程，循环接收它的所有内容
		pthread_t pid;
		pthread_create(&pid, NULL, wel, &user);
	}

	over();
	close(pd);
	return 0;
}
