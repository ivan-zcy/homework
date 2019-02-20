#!/bin/bash

#时间
time=`date +%Y-%m-%d__%H:%M:%S`

#CPU负载
fz=`cat /proc/loadavg | cut -d ' ' -f 1-3`

#CPU温度
wd=`cat /sys/class/thermal/thermal_zone0/temp`
wd=`echo "scale=2;${wd}/1000" | bc`

#警告
war=""
if [[ $(echo "$wd < 50" | bc) = 1 ]];then
    war="normal"
elif [[ $(echo "$wd <= 70" | bc) = 1 ]];then
    war="note"
else
    war="warning"
fi

#求CPU占用率
#sum1和idle1
eval $(cat /proc/stat | head -1 | awk -v sum1=0 -v idle1=0 \
    '{for (i=2;i<=8;i++) {sum1=sum1+$i} \
    printf("sum1=%d;idle1=%d;", sum1, $5)}')

#等待0.5s
sleep 0.5

#sum2和idle2
eval $(cat /proc/stat | head -1 | awk -v sum2=0 -v idle2=0 \
    '{for (i=2;i<=8;i++) {sum2=sum2+$i} \
    printf("sum2=%d;idle2=%d;", sum2, $5)}')

#cpu占用率
zyl=`echo "scale=4;(1-($idle2-$idle1)/($sum2-$sum1))*100" | bc`
zyl=`printf "%.2f" "$zyl"`

echo "$time $fz $zyl ${wd}°C $war"
