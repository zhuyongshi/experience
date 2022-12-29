#!/bin/sh
echo "udpate start!"

kw_path=/home/zws/Desktop/experience/test/key.txt
MM_st_path=/home/zws/Desktop/experience/VH/test/MM_st.txt
stash_path=/home/zws/Desktop/experience/VH/test/stash_path.txt
thread_num=4
kw=parotideus

./rpc_search $kw_path $MM_st_path $stash_path $thread_num $kw