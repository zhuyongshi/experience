/*
 * Created by yongshu zhu  on 12/11/2022
 * Email: 993015403@qq.com
 * 
 */

#include "VH.client.h"
#include "VH.Util.h"
#include <chrono> // 是一个time library ，源于boost库，已经是c++标准了
#include <bits/stdc++.h>

int main(int argc, char **argv)
{
    if (argc < 6)
    {
        std::cerr << "参数个数小于6个" << std::endl;
        exit(-1);
    } 
    size_t thread_num = atoi(argv[3]);
    //不同的数据集对应着不同的n、l
    int n = atoi(argv[6]);
    int l = atoi(argv[7]);
    std::string MM_st_path=argv[2];
    std::string order_MM_path = argv[3];
    int update_num = atoi(argv[5]);
    VH::Client client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()), std::string(argv[1]),MM_st_path,n,l);
    std::cout << "取order_map!" << std::endl;
    std::unordered_map<std::string,std::vector<std::string>> order_MM;
    VH::get_MM(order_MM_path,order_MM);

    std::string stash_path = argv[4];
    std::map<std::string,std::queue<std::pair<std::string,std::string>>> stash;
    VH::get_stash(stash_path,stash);

    std::cout << "update 文档个数：" << update_num << std::endl;
    int j = 0;
    for(auto i : order_MM){
        client.update_algorithm(i.first,i.second,stash,"0");
        j++;
        if(j>update_num) break;
    }
    
    std::cout << "update结束!" << std::endl;
    return 0;
}


//取数据进行update
/*
    ./rpc_setup [kw_path] [MM_st_path] [order_MM_path] [stash_path.txt] [update_num] [n] [l] [thread_num]
*/
