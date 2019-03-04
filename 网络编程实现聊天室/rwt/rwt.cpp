#include "./rwt.h"

//初始化信号量
void init() {
	readCount = 0;
	writeCount = 0;
	pthread_mutex_init(&rwt, NULL);
	pthread_mutex_init(&RWmutex, NULL);
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);
	pthread_mutex_init(&mutex3, NULL);
}

//关闭信号量
void over() {
	pthread_mutex_destroy(&rwt);
	pthread_mutex_destroy(&RWmutex);
	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
	pthread_mutex_destroy(&mutex3);
}

//读者
int reader(LIST *list) {
	init(list);

	//进队列
	pthread_mutex_lock(&mutex3);
	pthread_mutex_lock(&RWmutex);
	pthread_mutex_lock(&mutex2);
	readCount++;
	if (readCount == 1) {
		pthread_mutex_lock(&rwt);
	}
	pthread_mutex_unlock(&mutex2);
	pthread_mutex_unlock(&RWmutex);
	pthread_mutex_unlock(&mutex3);

	//读
	FILE *file = NULL;
	char *buffer = NULL;
	char *p = NULL;
	ssize_t len = 0;
	size_t chang = 0;

	//打开文件
	if ((file = fopen("../userList.txt", "a+")) == NULL) {
		perror("fopen faild\n");
		return -1;
	}

	//循环读入每行信息到buffer
	while((len = getline(&buffer, &chang, file)) != -1) {
		USER *user = (USER *)malloc(sizeof(USER));
		p = strtok(buffer, " ");
		strcpy(user -> name, p);
		p = strtok(NULL, "\n");
		user -> pd = atoi(p);
		user -> next = NULL;
		add(list, user);
		while(p != NULL) {
			p = strtok(NULL, "\n");
		}
	}
	fclose(file);

	//出队列
	pthread_mutex_lock(&mutex2);
	readCount--;
	if (readCount == 0) {
		pthread_mutex_unlock(&rwt);
	}
	pthread_mutex_unlock(&mutex2);
	return 0;
}

//写者
int writer(LIST *list) {
	//进队列
	pthread_mutex_lock(&mutex1);
	writeCount++;
	if (writeCount == 1) {
		pthread_mutex_lock(&RWmutex);
	}
	pthread_mutex_unlock(&mutex1);

	//写
	pthread_mutex_lock(&rwt);
	FILE *file = NULL;

	//打开文件
	if ((file = fopen("../userList.txt", "w+")) == NULL) {
		perror("fopen faild\n");
		return -1;
	}
	USER *p = list -> head;
	while(p != NULL) {
		fprintf(file, "%s %d\n", p -> name, p -> pd);
		p = p -> next;
	}
	fclose(file);	
	pthread_mutex_unlock(&rwt);

	//出队列
	pthread_mutex_lock(&mutex1);
	writeCount--;
	if (writeCount == 0) {
		pthread_mutex_unlock(&RWmutex);
	}
	pthread_mutex_unlock(&mutex1);
	return 0;
}

//线程执行函数
void* wel(void *t) {
	USER *user = (USER *)t;
	Message message;
	char buffer[300];
	LIST list;
	init(&list);
	strcpy(message.from, user -> name);

	//循环接收该用户发来的消息
	while(1) {
		//接收信息
		int recv_len = recv(user -> pd, buffer, 256, 0);
		if (recv_len == -1 || recv_len == 0) {
			break;
		}

		//重新更新链表
		if (reader(&list) == -1) {
			break;
		}
		//不同的输入执行服务器执行不同的操作
		if (strcmp(buffer, "#") == 0) {
			char s[256];
			query_now(&list, s);
			message.flag = 2;
			strcpy(message.message, s);
			query(&list, user -> name, &message);
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
				query(&list, user -> name, &message);
			}
		} else {
			message.flag = 0;
			strcpy(message.message, buffer);
			getout(&list, &message);
		}
		printf("\033[;31m%s:\033[0m %s\n", user -> name, buffer);
		fflush(stdout);
	}

	//删除该用户并更新到用户链表文件
	if (reader(&list) == -1) {
		printf("reader faild\n");
	}
	del(&list, user -> name);
	if (writer(&list) == -1) {
		printf("writer faild\n");
	}

	//在服务端显示用户退出
	printf("\033[;33m用户%s退出聊天室\n\033[0m", user -> name);

	//通知其他在线用户该用户已下线
	message.flag = 2;
	sprintf(message.message, "用户%s退出聊天室啦", user -> name);
	getout(&list, &message);

	//关闭连接并关闭线程
	close(user -> pd);
	pthread_exit(0);
}