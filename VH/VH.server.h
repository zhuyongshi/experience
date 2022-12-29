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
            std::unordered_map<std::string,std::string> DX;
            std::string dx_path;
            int sum;
        
        public:
            VHServiceImpl(const std::string db_path, int concurrent,const std::string dx_path_p)
            {
                signal(SIGINT, abort); //中断信号，缺省行为终止进程
                options.create_if_missing = true;
                Util::set_db_common_options(options); //数据库默认配置
                rocksdb::Status s1 = rocksdb::DB::Open(options, db_path, &ss_db);
                MAX_THREADS = concurrent; 
                DX["test"] = "test";
                dx_path=dx_path_p;
                sum = 0;
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
                    //std::cout<<tmp<<std::endl;
                }
                if (s.ok())
                    return tmp;
                else
                    return "";
            }
            
            //接收客户端传来的数据 存进数据库里
            // Status update_save_DB(ServerContext * context, ServerReader<UpdateRequestMessage> * reader, ExecuteStatus * response)
            // {
            //     std::string l;
            //     std::string e;
            //     //std::map<std::string,std::string> DX;
            //     struct timeval t1, t2;
            //     int sum = 0; 

            //     std::cout << "更新批量的键值对！" << std::endl;
            //     UpdateRequestMessage request;
            //     while (reader->Read(&request))
            //     {  
            //         l = request.l();
            //         e = request.e();
            //         //DX[l]=e;
            //         int status = store(ss_db, l, e);
            //         if (status != 0){
            //             response->set_status(false);
            //             return Status::CANCELLED;
            //         }
            //         sum++;
            //         //std::cout<<sum<<std::endl;
            //     }
            //     // TODO 读取之后需要解锁
            //     std::cout<<sum<<std::endl;
            //     response->set_status(true);
            //     // gettimeofday(&t1, NULL);
            //     // for(auto i :DX){
            //     //     int status = store(ss_db, i.first, i.second);
            //     //     if (status != 0)
            //     //     {
            //     //         response->set_status(false);
            //     //         return Status::CANCELLED;
            //     //     }
            //     // }
            //     // gettimeofday(&t2, NULL);
            //     // std::cout<<"server time:"<<((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0<< " ms" << std::endl;

            //     return Status::OK;
            // }   

            //接收客户端传来的数据 存进内存里
            Status batchupdate(ServerContext * context, ServerReader<UpdateRequestMessage> * reader, ExecuteStatus * response)
            {
                std::string l;
                std::string e;
                struct timeval t1, t2;
                std::cout << "更新批量的键值对！" << std::endl;
                UpdateRequestMessage request;
                int sum=0;

                while (reader->Read(&request))
                {  
                    l = request.l();
                    e = request.e();
                    DX[l]=e;
                    sum++;
                    // std::cout<<sum<<std::endl;
                }

                std::cout<<sum<<std::endl;
                write_dx_txt();
                response->set_status(true);
                return Status::OK;
            }

           //接收客户端传来的数据 存进内存里
            Status update(ServerContext * context, const UpdateRequestMessage *request, ExecuteStatus *response)
            {
                
                std::string l = request->l();
                std::string e = request->e();
                DX[l] = e;
                // std::cout<<"l = "<<l<<std::endl;
                // std::cout<<"e = "<<e<<std::endl;
                std::cout<<sum<<std::endl;
                sum++;
                return Status::OK;
            }

            void write_dx_txt(){
                std::ofstream	os(dx_path,std::ios::app);
                int sum = 0;
                for(auto i : DX){
                    sum++;
                    os<<i.first<<" "<<i.second<<"\n";
                }
                //std::cout<<sum<<std::endl;
                os.close();
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

            // Status search(ServerContext * context, const SearchRequestMessage *request,
            //               ServerWriter<SearchReply> *writer)
            // {
            //     std::cout<<"server search"<<std::endl;
            //     int q_f = request->q_f();
            //     int cnt = request->cnt();
            //     std::string x = request->x();
            //     std::vector<std::string> Result(cnt);
            //     struct timeval t1, t2;
            //     gettimeofday(&t1, NULL);
            //     for(int i=q_f,j=0;i<q_f+cnt;++i,++j){
            //         std::string y = Util::H_key(x,std::to_string(i));
            //         Result[j]= get(ss_db,y);
            //         std::cout<<i<<std::endl;
            //     }
            //     gettimeofday(&t2, NULL);
            //     std::cout<<"time1:"<<((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0<< " ms" << std::endl;
            //     //struct timeval t3, t4;
            //     //gettimeofday(&t3, NULL);
            //     for(auto i : Result){
            //         SearchReply reply;
            //         reply.set_ind(i);
            //         writer->Write(reply);
            //     }
            //     //gettimeofday(&t4, NULL);
            //     //std::cout<<"time:"<<((t4.tv_sec - t3.tv_sec) * 1000000.0 + t4.tv_usec - t3.tv_usec) / 1000.0<< " ms" << std::endl;

            //     std::cout<<"server search end!"<<std::endl;
            //     return Status::OK;
            // }

            Status search(ServerContext * context, const SearchRequestMessage *request,
                          ServerWriter<SearchReply> *writer)
            {
                Util::clear_txt(dx_path);
                write_dx_txt();
                std::cout<<"server search"<<std::endl;
                int q_f = request->q_f();
                int cnt = request->cnt();
                std::string x = request->x();
                std::vector<std::string> Result(cnt);
                struct timeval t1, t2;
                gettimeofday(&t1, NULL);
                for(int i=q_f,j=0;i<q_f+cnt;++i,++j){
                    std::string y = Util::H_key(x,std::to_string(i));
                    Result[j]= DX[y];
                    std::cout<<i<<std::endl;
                }
                gettimeofday(&t2, NULL);
                std::cout<<"time1:"<<((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0<< " ms" << std::endl;

                for(auto i : Result){
                    SearchReply reply;
                    reply.set_ind(i);
                    writer->Write(reply);
                }
                
                std::cout<<"server search end!"<<std::endl;
                return Status::OK;
            }


    };
}//namespace VH

    //静态成员在主函数外部声明
    rocksdb::DB *VH::VHServiceImpl::ss_db;
    void RunServer(std::string db_path, int concurrent,std::string dx_path)
    {
        std::string server_address("0.0.0.0:50051"); //可修改服务器地址0.0.0.0
        VH::VHServiceImpl service(db_path,concurrent,dx_path);
        ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout << "服务器监听地址：" << server_address << std::endl;
        server->Wait();
    }

#endif // VH_SERVER_H