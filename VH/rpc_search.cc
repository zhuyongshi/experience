/*
 * Created by yongshu zhu  on 12/11/2022
 * Email: 993015403@qq.com
 * 
 */

#include "VH.client.h"
#include <bits/stdc++.h>
#include <chrono> // 是一个time library ，源于boost库，已经是c++标准了

using VH::SearchRequestMessage;

int main(int argc, char **argv){
    if(argc<5){
        std::cerr << "参数个数小于9个" << std::endl;
        exit(-1);
    }
    size_t thread_num = atoi(argv[7]);
    std::string MM_st_path=argv[2];


    std::string stash_path = argv[4];
    std::map<std::string,std::queue<std::pair<std::string,std::string>>> stash;
    VH::get_stash(stash_path,stash);
    VH::Util::clear_txt(stash_path);
    VH::Client client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()), std::string(argv[1]),MM_st_path);

    std::string label = argv[5];
    std::unordered_set<std::string> Result;

    std::cout<<"search start"<<std::endl;
    client.search(label,stash,Result);
    std::cout<<"search end"<<std::endl;
    for(auto i : Result){
        std::cout<<i<<" ";
    }
    std::cout<<std::endl;
    VH::write_stash_txt(stash_path,stash);
    return 0;
}   

//普通搜索实现
//      0           1           2           3           4           5                
// /.rpc_search  [kw_path] [MM_st_path] [stash_path]  [threads_num] [kw]