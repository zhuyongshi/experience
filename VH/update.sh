#!/bin/sh
echo "udpate start!"

kw_path=/home/zws/Desktop/experience/dataset/50/key_2_14.txt
MM_st_path=/home/zws/Desktop/experience/VH/test/MM_st.txt
order_MM_path=/home/zws/Desktop/experience/test/data_3/up_2_22.txt
stash_path=/home/zws/Desktop/experience/VH/test/stash_path.txt
update_num=4194304
thread_num=4

./rpc_update $kw_path $MM_st_path $order_MM_path $stash_path $update_num $thread_num
