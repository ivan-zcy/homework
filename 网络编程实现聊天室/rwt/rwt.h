#ifndef _RWT_H_
#define _RWT_H_

#include "../list/list.h"

extern int readCount, writeCount;
extern pthread_mutex_t rwt, RWmutex, mutex1, mutex2, mutex3;

//初始化信号量
void init();

//关闭信号量
void over();

//读者
int reader(LIST *list);

//写者
int writer(LIST *list);

//线程执行函数
void* wel(void *t);

#endif