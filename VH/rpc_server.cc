/*
 * Created by yongshi zhu on 12/11/2022
 * Email: 993015403@qq.com
 * 
 */

#include "VH.server.h"

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "参数不足r 4个！" << std::endl;
        exit(-1);
    }
    RunServer(std::string(argv[1]), atoi(argv[3]),std::string(argv[2]));
}

// ./rpc_server [sdb_path] [dx_path] [threadsnum]
