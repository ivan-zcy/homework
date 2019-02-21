#!/bin/bash

#时间
time=`date +%Y-%m-%d__%H:%M:%S`

#用户总数
eval $(cat /etc/passwd | tr ':' ' ' | awk -v sum=0 '{if($3>1000){sum+=1;}} END{printf("sum=%d;", sum);}')

#最近活跃用户
act=(`last | cut -d ' ' -f 1 | head -n -2 | sort | uniq -c | sort -r | tr -s -c 'a-zA-Z0-9\n' ' ' | cut -d ' ' -f 3`)

#具有ROOT权限的用户
suname=`cat /etc/group | grep sudo | cut -d ':' -f 4`

#当前在线用户
eval $(w -h | awk -v online=0 '{online+=1;\
    printf("user["online"]=%s;ip["online"]=%s;tty["online"]=%s;",$1,$3,$2);} END{printf("online=%d;", online);}')

#输出信息
echo -n "$time $sum [${act[0]},${act[1]},${act[2]}] [$suname] ["
flag=0
for (( i=1;i<=online;i++ )); do
    if [[ flag -eq 0 ]];then
        echo -n "${user[$i]}_${ip[$i]}_${tty[$i]}"
        flag=1
    else
        echo -n ",${user[$i]}_${ip[$i]}_${tty[$i]}"
    fi
done
echo "]"
