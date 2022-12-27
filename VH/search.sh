#!/bin/sh
echo "udpate start!"

kw_path=/home/zws/Desktop/experience/dataset/50/key_04.txt
MM_st_path=/home/zws/Desktop/experience/VH/test/MM_st.txt
db_path=/home/zws/Desktop/experience/VH/sdb
stash_path=/home/zws/Desktop/experience/VH/test/stash_path.txt
l=2000
p=16384
thread_num=4
kw=thepeerage

./rpc_search $kw_path $MM_st_path $db_path $stash_path $l $p $thread_num $kw