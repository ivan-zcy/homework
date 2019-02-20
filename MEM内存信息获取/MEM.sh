#!/bin/bash

time=`date +%Y-%m-%d__%H:%M:%S`

total=`free | head -2 | tail -1 | awk '{printf("%d", $2)}'`
total=$[${total}/1000]

free=`free | head -2 | tail -1 | awk '{printf("%d", $4)}'`
free=$[${free}/1000]

used=$[$total-${free}]

bi=`echo "scale=1;100*${used}/${total}" | bc`

avgbi=`echo "scale=1;0.7*${bi}+0.3*$1" | bc`

echo "$time ${total}M $free ${bi}% ${avgbi}%"
