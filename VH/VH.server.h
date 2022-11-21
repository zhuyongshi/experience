/*
 * Created by yongshi zhu on 12/11/2022
 * Email: 993015403@qq.com
 * 
 */

#ifndef VH_SERVER_H
#define VH_SERVER_H

#include <grpc++/grpc++.h>
#include "VH.grpc.pb.h"
#include "VH.Util.h"
#include <unordered_set>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;

using namespace CryptoPP;

namespace VH{
    
    class VHServiceImpl final : public RPC::Service{
        private:
            static rocksdb::DB *ss_db;
            int MAX_THREADS; //最大线程数
            rocksdb::Options options;
        
        public:
            VHServiceImpl(const std::string db_path, int concurrent)
            {
                signal(SIGINT, abort); //中断信号，缺省行为终止进程
                options.create_if_missing = true;
                Util::set_db_common_options(options); //数据库默认配置
                rocksdb::Status s1 = rocksdb::DB::Open(options, db_path, &ss_db);
                MAX_THREADS = concurrent; 
            }
            static void abort(int signum)
            {
                delete ss_db;
                std::cout << "abort: " << signum << std::endl;
                exit(signum);
            }
            //将数据存到DB中
            static int store(rocksdb::DB *&db, const std::string l, const std::string e)
            {
                rocksdb::Status s;
                rocksdb::WriteOptions write_option = rocksdb::WriteOptions();
                {
                    s = db->Put(write_option, l, e);
                }
                if (s.ok())
                    return 0;
                else
                    return -1;
            }
            //通过键从DB中取出数据，后续需要做一个取出来并且将数据删除的操作
            static std::string get(rocksdb::DB *&db, const std::string l)
            {
                std::string tmp;
                rocksdb::Status s;
                {
                    s = db->Get(rocksdb::ReadOptions(), l, &tmp);
                }
                if (s.ok())
                    return tmp;
                else
                    return "";
            }
            
            //接收客户端传来的数据
            Status update(ServerContext * context, ServerReader<UpdateRequestMessage> * reader, ExecuteStatus * response)
            {
                std::string l;
                std::string e;
                // TODO 读取数据库之前要加锁，读取之后要解锁
                std::cout << "更新批量的键值对！" << std::endl;
                UpdateRequestMessage request;
                while (reader->Read(&request))
                {
                    l = request.l();
                    e = request.e();
                    store(ss_db, l, e);
                }
                // TODO 读取之后需要解锁
                response->set_status(true);
                return Status::OK;
            }   

            


    };
}//namespace VH

    //静态成员在主函数外部声明
    rocksdb::DB *VH::VHServiceImpl::ss_db;
    void RunServer(std::string db_path, int concurrent)
    {
        std::string server_address("0.0.0.0:50051"); //可修改服务器地址0.0.0.0
        VH::VHServiceImpl service(db_path,concurrent);
        ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout << "服务器监听地址：" << server_address << std::endl;
        server->Wait();
    }

#endif // VH_SERVER_H