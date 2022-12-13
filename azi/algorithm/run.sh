#!/bin/bash

# 编译可调试文件：
# sh run.sh debug
# 编译可执行文件：
# sh run.sh

dbstr="debug"

if [ "$1" = "$dbstr" ]
then
    g++ test.cpp pr_filter_util.cpp pr_filter_server.cpp pr_filter_client.cpp crypto_util.cpp -o run -lcryptopp -g3
    echo "编译可调试文件"
else
    g++ test.cpp pr_filter_util.cpp pr_filter_server.cpp pr_filter_client.cpp crypto_util.cpp -o run -lcryptopp -O3
    echo "编译可执行文件"
    echo "执行输出："
    ./run
    rm -rf run
fi


