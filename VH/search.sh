#!/bin/sh
echo "udpate start!"

kw_path=/home/zws/Desktop/experience/small04/key_01.txt
MM_st_path=/home/zws/Desktop/experience/VH/test/MM_st.txt
db_path=/home/zws/Desktop/experience/VH/sdb
stash_path=/home/zws/Desktop/experience/VH/test/stash_path.txt
l=1000
p=256
thread_num=4
kw=Dillon

./rpc_search $kw_path $MM_st_path $db_path $stash_path $l $p $thread_num $kw