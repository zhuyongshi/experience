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
    if (argc < 4)
    {
        std::cerr << "参数个数小于2个" << std::endl;
        exit(-1);
    }
    size_t thread_num = atoi(argv[3]);
    //不同的数据集对应着不同的n、l
    int n = 5;
    int l = 521;
    std::string MM_st_path=argv[2];
    VH::Client client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()), std::string(argv[1]),MM_st_path,n,l);
    std::cout << "setup开始！" << std::endl;
    client.setup();
    std::cout << "setup结束！" << std::endl;
    return 0;
}

//取数据进行setup
/*
    ./rpc_setup [kw_path] [MM_st_path] [thread_num]
*/
