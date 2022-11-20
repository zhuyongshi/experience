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

//用于生成子密钥，主密钥和初始化向量
byte k[17] = "0123456789abcdef";
byte iv_s[17] = "0123456789abcdef";

namespace VH{
    //若 frist + cnt_true > n 会出现漏掉真实文档的情况，这个比较危险。
    struct st{
        int frist;  //查询头，这个数字也代表着继上一次查询已更新的次数
        int cnt_true; //目前关键字对应的真实文档
        int cnt_up;    //当前对关键字的更新次数
        std::string key;
    };

    class Client{
        public:
        Client(std::shared_ptr<Channel> channel,std::string kw_path,int p_n,int p_l) : stub_(RPC::NewStub(channel))
        {
            get_w_array(kw_path,fullkw);
            l = p_l;
            n = p_n;
            
        }

        ~Client()
        {
           
        }

        void setup(std::string MM_st_path){
            std::map<std::string,std::string> DX;
            for(auto kw : fullkw){
                std::string kw_key = gen_kw_key(kw);
                for(int i=0;i<n;i++){
                    std::string x = Util::H1(kw+kw_key); //保护关键字
                    std::string y = Util::H1(x+ std::to_string(i));  //产生加密索引
                    std::string e_value; 
                    char op = '4'; //op 0：app 1：edit  2: rm 3: del 4:full
                    Util::encrypt(kw_key,std::to_string(i)+','+op,e_value); 
                    DX[y] = e_value;
                }
                MM_st[kw].key = kw_key;
                MM_st[kw].frist = 0;
                MM_st[kw].cnt_true = 0;
                MM_st[kw].cnt_up = n - 1;
                break;
            }
            std::vector<UpdateRequestMessage> update_list;
            gen_update_list(update_list,DX);
            std::cout<<"setup update data to server"<<std::endl;
            batch_update(update_list);
            std::cout<<"setup update finished"<<std::endl;
        }

        void get_w_array(std::string path, std::vector<std::string>& kw_set){
            std::ifstream key_myfile(path);
            std::string w;
            while (getline(key_myfile, w))								
            {
                std::stringstream input(w);
                std::string out;
                while (input >> out) {
                    kw_set.push_back(out);
                }
            }
        }


        void gen_update_list(std::vector<UpdateRequestMessage>& update_list,std::map<std::string,std::string> DX){
            for(auto i : DX){
                UpdateRequestMessage request;
                request.set_l(i.first);
                request.set_e(i.second);
                update_list.push_back(request);
            }
            
        }

        std::string gen_kw_key(const std::string kw){
            // 使用padding方式将所有字符串补齐到16的整数倍长度
            std::string kw_padding;
            std::string kw_key;
            try
            {
                CFB_Mode< AES >::Encryption e;
                e.SetKeyWithIV(k, AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE);
                kw_padding = Util::padding(kw);
                byte tmp_new_st[AES128_KEY_LEN];
                e.ProcessData(tmp_new_st, (byte*) kw_padding.c_str(), AES128_KEY_LEN);
                kw_key= std::string((const char*)tmp_new_st, AES128_KEY_LEN);
            }
            catch(const CryptoPP::Exception& e)
            {
                std::cerr << "in gen_kw_key() 在生成kw_key中出问题 " << e.what()<< std::endl;
                exit(1);
            }
            return kw_key;
        }

        Status batch_update(std::vector<UpdateRequestMessage> update_list) {
            UpdateRequestMessage request;

            ClientContext context;

            ExecuteStatus exec_status;

            std::unique_ptr<ClientWriterInterface<UpdateRequestMessage>> writer(stub_->update(&context, &exec_status));
            int i = 0;		
            while(i < update_list.size()){
                writer->Write(update_list[i]);
            }
            writer->WritesDone();
            Status status = writer->Finish();

            return status;
        }




        private:
            rocksdb::DB *db;
            std::map<std::string, st> MM_st; //client存的状态表
            std::vector<std::string> fullkw; //关键字全集
            std::unique_ptr<RPC::Stub> stub_;
            int n;  
            int l;
    };
}


#endif