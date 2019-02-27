#include "./list.h"

//链表的初始化
void init(LIST *list) {
	list -> head = NULL;
	list -> len = 0;
}

//链表的增加
void add(LIST *list, USER *user) {
	if (list -> head == NULL) {
		list -> head = user;
		list -> len ++;
		return;
	}
	USER *p = list -> head;
	while(p -> next != NULL) {
		p = p -> next;
	}
	p -> next = user;
	list -> len ++;
	return;
}

//链表的删除
int del(LIST *list, char *user) {
	if (list -> head == NULL) {
		printf("delete faild\n");
		return -1;
	}
	USER *p = list -> head;
	USER *old = p;
	if (strcmp(p -> name, user) == 0) {
		p = p -> next;
		list -> head = p;
		list -> len --;
		return 0;
	}
	while(p -> next != NULL) {
		p = p -> next;
		if (strcmp(p -> name, user) == 0) {
			old -> next = p -> next;
			list -> len --;
			return 0;
		}
		old = p;
	}
	printf("delete faild\n");
	return -1;
}

//将信息转发给所有在线用户
void getout(LIST *list, Message *message) {
	USER *p = list -> head;

	//循环给每个用户发送
	while(p != NULL) {
		//每次发送建立一个连接
		char *host = p -> host;
		int port = p -> port;
		int pd;
		if ((pd = socket_connect(port, host)) == -1) {
			return;
		}
		send(pd, message, sizeof(Message), 0);
		close(pd);
		p = p -> next;
	}
	return;
}

//将信息转发给指定用户，若返回-1需要告知
int query(LIST *list, char *user, Message *message) {
	USER *p = list -> head;

	//查询当前所有用户
	while(p != NULL) {
		//如果用户名相同，则发送信息并返回0(成功)
		if (strcmp(p -> name, user) == 0) {
			char *host = p -> host;
			int port = p -> port;
			int pd;
			if ((pd = socket_connect(port, host)) == -1) {
				return -1;
			}
			send(pd, message, sizeof(Message), 0);
			close(pd);
			return 0;
		}
		p = p -> next;
	}
	return -1;
}

//将信息转发给除该用户以外的所有用户
void query_other(LIST *list, char *user, Message *message) {
	USER *p = list -> head;
	//遍历所有当前用户
	while(p != NULL) {
		//如果用户名不相同，则发送信息
		if (strcmp(p -> name, user) != 0) {
			char *host = p -> host;
			int port = p -> port;
			int pd;
			if ((pd = socket_connect(port, host)) == -1) {
				return;
			}
			send(pd, message, sizeof(Message), 0);
			close(pd);
		}
		p = p -> next;
	}
	return;
}

//获取当前在线用户信息并存储到s
void query_now(LIST *list, char *s) {
	sprintf(s, "当前用户数量为%d, 分别是[ ", list -> len);
	USER *p = list -> head;
	while(p != NULL) {
		strcat(s, p -> name);
		strcat(s, " ");
		p = p -> next;
	}
	strcat(s, "]");
	return;
}