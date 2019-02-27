#ifndef _LIST_H_
#define _LIST_H_

#include "../common/common.h"

//用于服务端存储连接用户的信息
struct USER {
	char host[20];
	char name[20];
	int port;
	USER *next;
};

//链表存储当前在线的用户
struct LIST {
	USER *head;
	int len;
};

//链表的初始化
void init(LIST *list);

//链表的增加
void add(LIST *list, USER *user);

//链表的删除
int del(LIST *list, char *user);

//将信息转发给所有在线用户
void getout(LIST *list, Message * message);

//将信息转发给指定用户，若返回-1需要告知
int query(LIST *list, char *user, Message *message);

//将信息转发给除它以外的所有用户
void query_other(LIST *list, char *user, Message *message);

//获取当前在线用户信息并存储到s
void query_now(LIST *list, char *s);

#endif