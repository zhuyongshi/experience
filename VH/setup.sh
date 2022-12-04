#!/bin/sh
echo "setup start!"

kw_path=/home/zws/Desktop/experience/small04/key_01.txt 
MM_st_path=/home/zws/Desktop/experience/VH/test/MM_st.txt
n=465
l=329
thread_num=4

./rpc_setup $kw_path $MM_st_path $n $l $thread_num