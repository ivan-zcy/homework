#!/bin/bash

#时间
time=`date +%Y-%m-%d__%H:%M:%S`

#分区总量
sum=(`df | tr -s -c 'a-zA-Z0-9%-/\n' ' ' | cut -d ' ' -f 2 | tr -s -c 'a-zA-Z0-9-' ' '`)

#分区剩余量
sy=(`df | tr -s -c 'a-zA-Z0-9%-/\n' ' ' | cut -d ' ' -f 4 | tr -s -c 'a-zA-Z0-9' ' '`)

#分区占用比
bl=(`df | tr -s -c 'a-zA-Z0-9%-/\n' ' ' | cut -d ' ' -f 5 | tr -s -c 'a-zA-Z0-9%' ' '`)

#分区名
name=(`df | tr -s -c 'a-zA-Z0-9%-/\n' ' ' | cut -d ' ' -f 6 | tr -s -c 'a-zA-Z0-9/' ' '`)

#分区数
n=${#sum[*]}

#输出
sum1=0
sum2=0
sum3=0
for (( i=1;i<n;i++ ));do
    echo "$time 1 ${name[$i]} ${sum[$i]} ${sy[$i]} ${bl[$i]}"
    sum1=$[$sum1+${sum[$i]}]
    sum2=$[$sum2+${sy[$i]}]
done
sum3=$[$sum1-$sum2]
sum3=$[$sum3*100/$sum1]

echo "$time 0 disk $sum1 $sum2 ${sum3}%"
