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
                if (s.ok()){
                    // std::cout<<l<<" "<<e<<std::endl;
                    return 0;
                }
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
                    std::cout<<tmp<<std::endl;
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
                //std::map<std::string,std::string> DX;
                struct timeval t1, t2;
                int sum = 0; 

                std::cout << "更新批量的键值对！" << std::endl;
                UpdateRequestMessage request;
                while (reader->Read(&request))
                {  
                    l = request.l();
                    e = request.e();
                    //DX[l]=e;
                    int status = store(ss_db, l, e);
                    if (status != 0){
                        response->set_status(false);
                        return Status::CANCELLED;
                    }
                    sum++;
                    //std::cout<<sum<<std::endl;
                }
                // TODO 读取之后需要解锁
                std::cout<<sum<<std::endl;
                response->set_status(true);
                // gettimeofday(&t1, NULL);
                // for(auto i :DX){
                //     int status = store(ss_db, i.first, i.second);
                //     if (status != 0)
                //     {
                //         response->set_status(false);
                //         return Status::CANCELLED;
                //     }
                // }
                // gettimeofday(&t2, NULL);
                // std::cout<<"server time:"<<((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0<< " ms" << std::endl;

                return Status::OK;
            }   

            Status updateDX(ServerContext * context, const UpdateDXMessage *request, ExecuteStatus *response)
            {
                for(int i=0;i<request->e_dx_size();i++){
                    PairMessage e_pair1 = request->e_dx(i); 
                    int status = store(ss_db, e_pair1.l(), e_pair1.e());
                    if (status != 0)
                    {
                        response->set_status(false);
                        return Status::CANCELLED;
                    }
                    response->set_status(true);
                    return Status::OK;
                }
                return Status::OK;
                
            }

            Status search(ServerContext * context, const SearchRequestMessage *request,
                          ServerWriter<SearchReply> *writer)
            {
                std::cout<<"server search"<<std::endl;
                int q_f = request->q_f();
                int cnt = request->cnt();
                std::string l = request->x();
                std::string Result;
                for(int i=q_f;i<q_f+cnt;++i){
                    std::string y = Util::H_key(l,std::to_string(i));
                    Result = get(ss_db,y);
                    // std::cout<<Result<<std::endl;
                    SearchReply reply;
                    reply.set_ind(Result);
                    writer->Write(reply);
                }
                   
                std::cout<<"server search end!"<<std::endl;
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