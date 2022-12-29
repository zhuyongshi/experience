/*
 * Created by yongshi zhu  on 12/11/2022
 * Email: 993015403@qq.com
 * 
 */

#ifndef VH_CLIENT_H
#define VH_CLIENT_H
#include <bits/stdc++.h>
#include <grpc++/grpc++.h>
#include <VH.Util.h>
#include "VH.grpc.pb.h"
#include <cryptopp/randpool.h>
//#include "omp.h"
using grpc::Channel;
using grpc::ClientAsyncResponseReaderInterface;
using grpc::ClientContext;
using grpc::ClientReaderInterface;
using grpc::ClientWriterInterface;

using grpc::Status;

std::string k = "0123456789abcdef";
std::string iv = "0123456789abcdef";
std::string K_enc = "abcdefghijklmnopq";

namespace VH{
    
    struct st{
        int cnt_q;  //查询次数
        int cnt; //当前版本下的更新次数
    };

    class Client{
        public:
        Client(std::shared_ptr<Channel> channel,std::string kw_path,std::string MM_st_path) : stub_(RPC::NewStub(channel))
        {
            srand(time(0)); 
            get_w_array(kw_path,fullkw);
            MM_st_txt_path = MM_st_path;
            //如果这个文件不存在的话是没办法读MM_st的
            //即 调用构造函数构造client对象的时候，如果MM_st不存在会走下面的else
            //读取了以后，将MM_st.txt的内容清空，析构函数会将更新锅的MM_st重新写入
            if(Util::file_exist(MM_st_txt_path)){
                std::cout<<"MM_st_txt exist"<<std::endl;
                get_MM_st(MM_st_txt_path);
                Util::clear_txt(MM_st_txt_path);
            }else{
                std::cout<<"MM_st_txt init"<<std::endl;
                init_MM_st();
            }
            std::cout<<"client的构造函数结束"<<std::endl;
        }

        ~Client()
        {
            std::cout<<"client的析构函数"<<std::endl;
            std::ofstream	os(MM_st_txt_path,std::ios::app);
           //将MM_st写回到文档里
            for(auto i : MM_st){
                os<<i.first<<" "<<i.second.cnt_q<<" "<<i.second.cnt<<"\n";
             }
            os.close();
        }
       



        Status updateDX(UpdateDXMessage updateDX)
        {

            ClientContext context;

            ExecuteStatus exec_status;
            // 执行RPC
            Status status = stub_->updateDX(&context, updateDX, &exec_status);
            assert(status.ok());

            return status;
        }


        void update_algorithm(std::string label,std::string v,
                              std::map<std::string,std::queue<std::pair<std::string,std::string>>> &stash,
                              std::string op)
        {
            struct timeval t1, t2;
            std::string random_label;
            gettimeofday(&t1, NULL);
            random_label = gen_random_label();
            std::pair<std::string,std::string> up;
            write_stash_and_gen_up(random_label,label,v,op,stash,up);

            std::string x = Util::H_key(k,random_label+std::to_string(MM_st[random_label].cnt_q)); //保护关键字
            std::string y = Util::H_key(x,std::to_string(MM_st[random_label].cnt));  //产生加密索引
            std::string e_value; 
            Util::encrypt(K_enc,iv,up.first+','+up.second,e_value);
            MM_st[random_label].cnt++;

            UpdateRequestMessage request;
            request.set_l(y);
            request.set_e(e_value);
            
            gettimeofday(&t2, NULL);
            //std::cout<<"client_update time:"<<((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0<< " ms" << std::endl;

            //std::cout<<"update data to server"<<std::endl;
            Status status = update(request);
            //std::cout<<"update finished"<<std::endl;
        }

        

        void write_stash_and_gen_up(std::string random_label,std::string label,std::string v,std::string op, 
                                    std::map<std::string,std::queue<std::pair<std::string,std::string>>> &stash,
                                    std::pair<std::string,std::string> &up)
        {   

            std::pair<std::string,std::string> temp_op_v({op,v});
            if(random_label != label){
                if(stash.find(label) == stash.end()){
                    std::queue<std::pair<std::string,std::string>> temp;
                    temp.push(temp_op_v);
                    stash[label] = temp;
                }else{
                    stash[label].push(temp_op_v);
                }

                if(stash.find(random_label)!= stash.end() && stash[random_label].size()>0){ 
                    std::pair<std::string,std::string> temp;
                    up = stash[random_label].front();
                    stash[random_label].pop();
                }else{
                    up = temp_op_v;
                    up.second+="*"; //代表这是假的信息。
                }
            }else{
                up = temp_op_v;
            }

        }        

        std::string gen_random_label(){
            int temp = rand() % fullkw.size();
            return fullkw[temp];
        }

        void init_MM_st(){
            for(auto kw : fullkw){
                MM_st[kw].cnt_q = 0;
                MM_st[kw].cnt = 0;
            }
        }

        void get_MM_st(std::string path){
            std::ifstream MM_myfile(path);
            std::string w,line;
            std::cout<<"open file success"<<std::endl;
            while (getline(MM_myfile, line))								
            {
                std::stringstream input(line);
                std::string out;
                input >> out;
                w = out;
                input >> out;
                MM_st[w].cnt_q = stoi(out);
                input >> out;
                MM_st[w].cnt=stoi(out);
            }
            MM_myfile.close();

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
            key_myfile.close();
        }

        Status update(UpdateRequestMessage update) {
            ClientContext context;

            ExecuteStatus exec_status;
            Status status = stub_->update(&context, update, &exec_status);
            if (status.ok())
                assert(status.ok());
            return status;
        }


        Status batch_update(std::vector<UpdateRequestMessage> update_list) {
            UpdateRequestMessage request;

            ClientContext context;
            ExecuteStatus exec_status;

            std::unique_ptr<ClientWriterInterface<UpdateRequestMessage>> writer(stub_->batchupdate(&context, &exec_status));

            int i = 0;

            while(i < update_list.size()){
                writer->Write(update_list[i]);
                ++i;
            }

            writer->WritesDone();
            Status status = writer->Finish();

            return status;
        }

        void search(const std::string label,std::map<std::string,std::queue<std::pair<std::string,std::string>>> &stash,std::unordered_set<std::string>& ID){
            std::cout<<"client search"<<std::endl;
            struct timeval t1,t2;
            gettimeofday(&t1, NULL);
            std::string x = Util::H_key(k,label+std::to_string(MM_st[label].cnt_q)); 
            SearchRequestMessage request;
            request.set_cnt(MM_st[label].cnt);
            request.set_x(x);

            ClientContext context;
            std::unique_ptr<ClientReaderInterface<SearchReply>> reader = stub_->search(&context, request);
            gettimeofday(&t2, NULL);
            std::cout<<"time1:"<<((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0<< " ms" << std::endl;


           //这里获得了从server端返回的真实且经过op操作以后的id列表。
           //读取返回列表：
            SearchReply reply;
            
            std::vector<std::string> response(MM_st[label].cnt+1);
            int i = 0;
            while(reader->Read(&reply)){
                response[i] = reply.ind();
                i++;
            }
            struct timeval t3;
            gettimeofday(&t3, NULL);
            for(auto r : response){
                std::string op_id;
                Util::descrypt(K_enc,iv,r,op_id);
                //std::cout<<op_id<<std::endl;
                if(op_id.find('*')<0){
                    //找不到‘*’代表是真的
                    int i = op_id.find(',');
                    std::string id = op_id.substr(i+1,op_id.size()) ;
                    std::string op = op_id.substr(0,i);
                    if(op == "0")  ID.insert(id);
                    else if(op == "1"){
                        if(ID.find(id) != ID.end())  ID.erase(id);
                    }
                }
            }
            
            //从stash中把跟kw相关的取出来。
            while(stash[label].size()!=0){
                std::pair<std::string,std::string> temp = stash[label].front();;
                if(temp.first == "0")   ID.insert(temp.second);
                else if(temp.second == "1"){
                    if(ID.find(temp.second) != ID.end())  ID.erase(temp.second);
                }
                stash[label].pop();
            }
            MM_st[label].cnt_q++;
            re_update(ID,label,t3,stash);

        }

        void re_update(const std::unordered_set<std::string> ID,const std::string label, timeval t1,std::map<std::string,std::queue<std::pair<std::string,std::string>>> &stash){
            struct timeval t2;
            std::vector<std::string> ID_arr(ID.begin(),ID.end());
            while(ID_arr.size()>MM_st[label].cnt){
                stash[label].push({"0",ID_arr[ID_arr.size()-1]});
            }
            while(ID_arr.size()<MM_st[label].cnt){
                ID_arr.push_back("*");
            }
            std::vector<UpdateRequestMessage> update_list(MM_st[label].cnt);
            int i = 0;
            for(auto j : ID_arr){
                std::string x = Util::H_key(k,label+std::to_string(MM_st[label].cnt_q)); //保护关键字
                std::string y = Util::H_key(x,std::to_string(i));  //产生加密索引
                std::string e_value; 
                Util::encrypt(K_enc,iv,"0,"+j,e_value);
                UpdateRequestMessage request;
                request.set_l(y);
                request.set_e(e_value);
                update_list[i] = request;
                i++;
            }
            gettimeofday(&t2, NULL);
            std::cout<<"time2:"<<((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0<< " ms" << std::endl;
            
            std::cout<<"re_update  data to server"<<std::endl;
            Status status = batch_update(update_list);
            std::cout<<"re_update finished"<<std::endl;
        }         



        private:
            std::map<std::string, st> MM_st; //client存的状态表
            std::vector<std::string> fullkw; //关键字全集
            std::unique_ptr<RPC::Stub> stub_;
            std::string MM_st_txt_path;
    };


    //从文件中取出顺序表的方法
    void get_MM(std::string path,std::unordered_map<std::string,std::vector<std::string>>& MM){
        std::ifstream MM_myfile(path);
        std::string id,line;
        while (getline(MM_myfile, line))								
        {
            std::stringstream input(line);
            std::string out;
            std::vector<std::string> ws;
            input >> out;
            id = out;
            while (input >> out) {
                ws.push_back(out);
            }
            MM[id] = ws;
        }
    }

    void get_update_pair(std::string path,std::vector<std::pair<std::string,std::string>> &up){
        std::ifstream MM_myfile(path);
        std::string line,label,v;
        int i=0;
        while (getline(MM_myfile, line))								
        {
            std::stringstream input(line);
            std::string out;
            input >> out;
            label = out;
            input >> out;
            v = out;
            up[i].first = label;
            up[i].second = v;
            i++;
        }
    }

    //从文件中取出stash的方法
    void get_stash(std::string path,std::map<std::string,std::queue<std::pair<std::string,std::string>>> &stash){
        std::ifstream myfile(path);
        std::string line,kw;
        while (getline(myfile, line))								
        {
            std::stringstream input(line);
            std::string out;
            std::pair<std::string,std::string> op_id;
            std::queue<std::pair<std::string,std::string>> ids;
            input >> out;
            kw = out;
            while (input >> out) {
                op_id.first = out;
                input >> out;
                op_id.second = out;
                ids.push(op_id);
            }
            stash[kw] = ids;
        }
        myfile.close();
    }

    //将stash写入文件
    void write_stash_txt(std::string path,std::map<std::string,std::queue<std::pair<std::string,std::string>>> &stash){
    std::ofstream	os(path,std::ios::app);
    for(auto i : stash){
        os<<i.first<<" ";
        while(i.second.size()>0){
            os<<i.second.front().first<<" "<<i.second.front().second<<" ";
            i.second.pop();
        }
        os<<"\n";
    }
    os.close();
    }
}


#endif