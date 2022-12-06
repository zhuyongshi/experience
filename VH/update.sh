#!/bin/sh
echo "udpate start!"

kw_path=/home/zws/Desktop/experience/small04/key_01.txt
MM_st_path=/home/zws/Desktop/experience/VH/test/MM_st.txt
order_MM_path=/home/zws/Desktop/experience/small04/ans_01
stash_path=/home/zws/Desktop/experience/VH/test/stash_path.txt
update_num=200
l=465
p=329
thread_num=4

./rpc_update $kw_path $MM_st_path $order_MM_path $stash_path $update_num $l $p $thread_num
