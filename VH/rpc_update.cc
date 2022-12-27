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
    if (argc < 9)
    {
        std::cerr << "参数个数小于6个" << std::endl;
        exit(-1);
    } 
    size_t thread_num = atoi(argv[6]);

    std::string MM_st_path=argv[2];
    std::string order_MM_path = argv[3];
    int update_num = atoi(argv[5]);
    VH::Client client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()), std::string(argv[1]),MM_st_path);
    std::cout << "取order_map!" << std::endl;
    std::unordered_map<std::string,std::vector<std::string>> order_MM;
    VH::get_MM(order_MM_path,order_MM);

    std::string stash_path = argv[4];
    std::map<std::string,std::queue<std::pair<std::string,std::string>>> stash;
    VH::get_stash(stash_path,stash);
    VH::Util::clear_txt(stash_path);

    std::cout << "the number of updated pairs" << update_num << std::endl;
    int j = 0;
    for(auto i : order_MM){
        for(auto z : i.second){
            client.update_algorithm(i.first,z,stash,"0",j);
            j++;
            if(j>=update_num) break;
        }
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
