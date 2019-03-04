#!/bin/bash

if [ "$1" = "DEBUG" ];then
    cd common/
    g++ common.cpp -D DEBUG -c

    cd ../list/
    g++ list.cpp -D DEBUG -c

    cd ../rwt/
    g++ rwt.cpp -D DEBUG -c

    cd ..
    g++ Socket.cpp -D DEBUG -c
    g++ User.cpp -D DEBUG -c

    g++ User.o common/common.o -o User
    g++ Socket.o common/common.o list/list.o rwt/rwt.o -o Socket
else
    cd common/
    g++ common.cpp -c

    cd ../list/
    g++ list.cpp -c

    cd ../rwt/
    g++ rwt.cpp -c

    cd ..
    g++ Socket.cpp -c
    g++ User.cpp -c

    g++ User.o common/common.o -o User
    g++ Socket.o common/common.o list/list.o rwt/rwt.o -o Socket
fi 
