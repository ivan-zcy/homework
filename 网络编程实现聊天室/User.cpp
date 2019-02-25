#include "./common/common.h"

int main() {
	char conf_file[20] = "./User.conf";
	char Server_Ip[20] = "Server_Ip";
	char Server_Port[20] = "Server_Port";
	char My_Name[20] = "My_Name";
	char Clinet_Port[20] = "Clinet_Port";
	char Log_file[20] = "Log_file";
	int pid = fork(); 
	if (pid < 0) {
		perror("fork faild");
		exit(1);
	}
	if (!pid) {
		char *host = (char *)malloc(sizeof(char) * 25);
		char *s = (char *)malloc(sizeof(char) * 1024);
		char *port_string = (char *)malloc(sizeof(char) * 10);
		char *name = (char *)malloc(sizeof(char) * 105);
		if (get_conf_val(conf_file, Server_Ip, host) == -1) {
			exit(1);
		}
		if (get_conf_val(conf_file, Server_Port, port_string) == -1) {
			exit(1);
		}
		if (get_conf_val(conf_file, My_Name, name) == -1) {
			exit(1);
		}
		int port = atoi(port_string);
		int pd = socket_connect(port, host);
		if (pd == -1) {
			free(s);
			free(host);
			free(port_string);
			free(name);
			exit(1);
		}
		send(pd, name, 105, 0);
		while(strcmp(s, "quit") != 0) {
			scanf("%[^\n]s", s);
			fflush(stdin);
			getchar();
			send(pd, s, 1024, 0);
		}
		DBG("子进程退出\n");
		free(host);
		free(s);
		free(port_string);
		free(name);
    	close(pd);
    	exit(0);
	} else {
		char *port_string = (char *)malloc(sizeof(char) * 10);
		char *Log_file = (char *)malloc(sizeof(char) * 105);
		if (get_conf_val(conf_file, Clinet_Port, port_string) == -1) {
			free(port_string);
			free(Log_file);
			exit(1);
		}
		if (get_conf_val(conf_file, Log_file, Log_file) == -1) {
			free(port_string);
			free(Log_file);
			exit(1);
		}
		int port = atoi(port_string);
		free(port_string);
		DBG("%d\n", port);
		int pd = socket_creat(port);
		if (pd == -1) {
			free(Log_file);
			exit(1);
		}
		struct sockaddr_in addr_user;
		bzero(&addr_user, sizeof(addr_user));
		signal(SIGCHLD, handle);
		while(true) {
			DBG("循环接收\n");
			int len = sizeof(addr_user);
			int newpd = accept(pd, (struct sockaddr *)&addr_user, (socklen_t *)&len);
			DBG("成功接收\n");
			if (newpd == -1) {
				perror("accept faild");
				continue;
			}
			int pid_son = fork();
			if (pid_son < 0) {
				perror("fork faild");
				continue;
			}
			if (!pid_son) {
				close(pd);
				Message *buffer = (Message *)malloc(sizeof(Message));
				while(1) {
					int recv_len = recv(newpd, buffer, sizeof(Message), 0);
					if (recv_len == -1 || recv_len == 0) {
						break;
					}
					recv_len = write_log(buffer, Log_file);
					if (recv_len == -1) {
						free(buffer);
						close(newpd);
						exit(1);
					}
				}
				free(buffer);
				close(newpd);
				exit(0);
			}
			close(newpd);
		}
		free(Log_file);
		close(pd);
	}
	return 0;
}