/*
 * Created by yongshi zhu  on 12/11/2022
 * Email: 993015403@qq.com
 * 
 */

#ifndef VH_CLIENT_H
#define VH_CLIENT_H
#include <bits/stdc++.h>
#include <rocksdb/db.h>
#include <grpc++/grpc++.h>
#include <VH.Util.h>
#include "VH.grpc.pb.h"
#include "logger.h"
using grpc::Channel;
using grpc::ClientAsyncResponseReaderInterface;
using grpc::ClientContext;
using grpc::ClientReaderInterface;
using grpc::ClientWriterInterface;

using grpc::Status;

namespace VH{
    struct st{
        int frist;
        int cnt_up;
        int cnt;
        byte k[17];
    };

    class Client{
        public:
        Client(std::shared_ptr<Channel> channel, std::string db_path) : stub_(RPC::NewStub(channel))
        {
            
        }

        ~Client()
        {
           
        }


        private:
            rocksdb::DB *db;
            std::map<std::string, st> MM_st; //client存的状态表
            std::unique_ptr<RPC::Stub> stub_;//这里需要生成proto文件才行，先忽略。
    };
}


#endif