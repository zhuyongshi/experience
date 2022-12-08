#!/bin/sh
echo "server start!"

sdb_path="/home/zws/Desktop/experience/VH/sdb"
dx_path="/home/zws/Desktop/experience/VH/ram/dx.txt"
threadsnum=4

./rpc_server $sdb_path $dx_path $threadsnum