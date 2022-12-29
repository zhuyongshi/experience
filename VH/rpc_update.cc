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
    srand((unsigned)time(NULL));
    if (argc < 6)
    {
        std::cerr << "参数个数小于6个" << std::endl;
        exit(-1);
    } 
    size_t thread_num = atoi(argv[6]);

    std::string MM_st_path=argv[2];
    std::string order_MM_path = argv[3];
    int update_num = atoi(argv[5]);
    VH::Client client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()), std::string(argv[1]),MM_st_path);
    std::cout << "取update pair!" << std::endl;
    std::vector<std::pair<std::string,std::string>> up_pair(update_num);
    VH::get_update_pair(order_MM_path,up_pair);

    std::string stash_path = argv[4];
    std::map<std::string,std::queue<std::pair<std::string,std::string>>> stash;
    VH::get_stash(stash_path,stash);
    VH::Util::clear_txt(stash_path);
    int sum = 0;
    for(auto i :  up_pair){
        //std::cout<<i.first<<" "<<i.second<<std::endl;
        std::cout<<++sum<<std::endl;
        client.update_algorithm(i.first,i.second,stash,"0");
    }
    

    VH::write_stash_txt(stash_path,stash);
    std::cout << "update end!" << std::endl;
    return 0;
}


//取数据进行update
/*
        0           1           2           3               4               5          6   
    ./rpc_update [kw_path] [MM_st_path] [order_MM_path] [stash_path] [update_num]  [thread_num]
*/
