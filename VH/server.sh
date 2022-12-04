#!/bin/sh
echo "server start!"

sdb_path="/home/zws/Desktop/experience/VH/sdb"
threadsnum=4

./rpc_server $sdb_path $threadsnum