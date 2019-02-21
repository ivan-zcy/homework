#!/bin/bashi

#时间
time=`date +%Y-%m-%d__%H:%M:%S`

#第一次获取占用CPU超过百分之50的进程
eval $(ps -aux -h --sort=-%cpu | awk -v cpusum=0 '{if($3>=50) {cpusum+=1; printf("cpu["cpusum"]=%d;", $2);} else {exit;}} END{printf("cpusum=%d;", cpusum);}')

#第一次获取占用内存超过百分之50的进程
eval $(ps -aux -h --sort=-%mem | awk -v memsum=0 '{if($4>=50) {memsum+=1; printf("mem["memsum"]=%d;", $2);} else {exit;}} END{printf("memsum=%d;", memsum);}')

#如果第一次检查存在cpu或内存占用超过百分之50，则等待5s，否则退出
if [[ $cpusum -gt 0 || $memsum -gt 0 ]];then
    sleep 5
else 
    exit 0
fi

#若存在占用cpu超过百分之50的进程，再次进行检查
if [[ $cpusum -gt 0 ]];then
    for i in ${cpu[*]};do
        eval $(ps -aux -q $i -h | awk -v cpucount=0 '{if($3>=50){cpucount+=1;printf("cpu2["cpucount"]=%d;cpu1["cpucount"]=%.40s;cpu3["cpucount"]=%s;cpu4["cpucount"]=%2.2f;cpu5["cpucount"]=%2.2f;",$2,$11,$1,$3,$4);}} END{printf("cpucount=%d;", cpucount);}')
    done
fi

#若存在占用内存超过百分之50的进程，再次进行检查
if [[ $memsum -gt 0 ]];then
    for i in ${mem[*]};do
        eval $(ps -aux -q $i -h | awk -v memcount=0 '{if($3>=50){memcount+=1;printf("mem2["memcount"]=%d;mem1["memcount"]=%.40s;mem3["memcount"]=%s;mem4["memcount"]=%2.2f;mem5["memcount"]=%2.2f;",$2,$11,$1,$3,$4);}} END{printf("memcount=%d;", memcount);}')
    done
fi

#若再次检查都不超过百分之50，系统结束
if [[ $cpucount -eq 0 && $memcount -eq 0 ]];then
    exit 0
fi

#若再次检查cpu还超过百分之50，则输出该进程的相关信息
if [[ $cpucount -gt 0 ]];then
    for (( i=1;i<=$cpucount;i++ ));do
        echo "$time ${cpu1[$i]} ${cpu2[$i]} ${cpu3[$i]} ${cpu4[$i]}% ${cpu5[$i]}%"
    done
fi

#若再次检查内存还超过百分之50，则输出该进程的相关信息
if [[ $memcount -gt 0 ]];then
    for (( i=1;i<=$memcount;i++ ));do
        echo "$time ${mem1[$i]} ${mem2[$i]} ${mem3[$i]} ${mem4[$i]}% ${mem5[$i]}%"
    done
fi

