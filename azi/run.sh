#!/bin/bash

# 编译可调试文件：
# sh run.sh debug
# 编译可执行文件：
# sh run.sh

dbstr="debug"

if [ "$1" = "$dbstr" ]
then
    g++ main.cpp pr_filter.cpp -o run -lcryptopp -g3
    echo "编译可调试文件"
else
    g++ main.cpp pr_filter.cpp -o run -lcryptopp
    echo "编译可执行文件"
fi

echo "执行输出："
./run
