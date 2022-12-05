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
using grpc::Channel;
using grpc::ClientAsyncResponseReaderInterface;
using grpc::ClientContext;
using grpc::ClientReaderInterface;
using grpc::ClientWriterInterface;

using grpc::Status;

//用于生成子密钥，主密钥和初始化向量
byte k[17] = "0123456789abcdef";
std::string iv = "0123456789abcdef";
std::string K_enc = "abcdefghijklmnopq";

namespace VH{
    //若 frist + cnt_true > n 会出现漏掉真实文档的情况，这个比较危险，需要控制
    struct st{
        int query_first;  //查询头，这个数字也代表着继上一次查询已更新的次数
        int cnt_true; //目前关键字对应的真实文档
        std::string key;
    };

    class Client{
        public:
        Client(std::shared_ptr<Channel> channel,std::string kw_path,std::string MM_st_path,int p_l,int p_p) : stub_(RPC::NewStub(channel))
        {
            srand(time(0)); 
            get_w_array(kw_path,fullkw);
            p = p_p;
            l = p_l;
            MM_st_txt_path = MM_st_path;
            //如果这个文件不存在的话是没办法读MM_st的
            //即 setup 时调用构造函数构造client对象的时候，会走下面的else
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
                os<<i.first<<" "<<i.second.query_first<<" "<<i.second.cnt_true<<" "<<i.second.key<<"\n";
             }
            os.close();
        }
       
        void setup(){
            std::cout<<"setup()"<<std::endl;
            std::ofstream	os(MM_st_txt_path,std::ios::app);
            
            std::cout<<"setup update data to server"<<std::endl;
            for(auto kw : fullkw){
                std::map<std::string,std::string> DX;
                for(int i=0;i<l;i++){
                    std::string x = Util::H_key(MM_st[kw].key,kw); //保护关键字
                    std::string y = Util::H_key(x,std::to_string(i));  //产生加密索引
                    std::string e_value; 
                    std::string op = "*2"; //op取值 0：app 1: del 2:full
                    Util::encrypt(K_enc,iv,op,e_value); 
                    DX[y] = e_value;
                }
                std::vector<UpdateRequestMessage> update_list;
                gen_update_list(update_list,DX);
                std::cout<<"update_list.size = "<<update_list.size()<<std::endl;
                Status status = update(update_list);
            }
            os.close();
            std::cout<<"setup update finished"<<std::endl;

        }

        void update_algorithm(std::string id,std::vector<std::string> &kw_arr,
                              std::map<std::string,std::queue<std::pair<std::string,std::string>>> &stash,
                              std::string op,int update_num)
        {
            std::vector<std::string> random_kw(p);
            gen_random_kw_array(random_kw,p);
            std::vector<std::pair<std::string,std::string>> up_op_id(p);
            write_stash_and_gen_up(random_kw,kw_arr,id,op,stash,up_op_id);
            int i=0;
            std::map<std::string,std::string> DX;
            for(auto kw : random_kw){
                if(MM_st[kw].query_first+MM_st[kw].cnt_true+1>l) std::cout<<"目前更新了:"<<update_num<<"个文档"<<std::endl;
                if(MM_st[kw].cnt_true > l) std::cout<<"需要重新构建数据库"<<std::endl;
                std::string x = Util::H_key(MM_st[kw].key,kw); //保护关键字
                std::string y = Util::H_key(x,std::to_string(MM_st[kw].query_first+l+1));  //产生加密索引
                std::string e_value; 
                Util::encrypt(K_enc,iv,up_op_id[i++].first+','+up_op_id[i++].second,e_value);
                MM_st[kw].query_first++;
                DX[y] = e_value;
            }
            std::vector<UpdateRequestMessage> update_list;
            gen_update_list(update_list,DX);
            std::cout<<"update_list.size = "<<update_list.size()<<std::endl;
            std::cout<<"update data to server"<<std::endl;
            Status status = update(update_list);
            std::cout<<"update finished"<<std::endl;
        }

        

        void write_stash_and_gen_up(std::vector<std::string>& random_kw,std::vector<std::string>& kw,std::string id,
                                    std::string op,std::map<std::string,std::queue<std::pair<std::string,std::string>>> &stash,
                                    std::vector<std::pair<std::string,std::string>> &up_op_id)
        {
            std::pair<std::string,std::string> temp_op_id({op,id});
            std::unordered_set<std::string> set_random_kw(random_kw.begin(),random_kw.end());
            std::unordered_set<std::string> set_kw(kw.begin(),kw.end());

            //kw中的元素没在抽样列表里面就放进stash里面
            for(int i=0;i<kw.size();i++){
                if(set_random_kw.find(kw[i]) == set_random_kw.end()){
                    if(stash.find(kw[i])==stash.end()){
                        std::queue<std::pair<std::string,std::string>> temp;
                        temp.push(temp_op_id);
                        stash[kw[i]] = temp;
                    }else{
                        stash[kw[i]].push(temp_op_id);
                    }
                }
            }
            
            //在抽样列表里面，不在真kw里面，又在stash里面，则从stash里面拿出来
            for(int i=0;i<random_kw.size();i++){
                if(stash.find(random_kw[i])!=stash.end() && stash[random_kw[i]].size()>0 && set_kw.find(random_kw[i])==set_kw.end()){
                    up_op_id.push_back(stash[random_kw[i]].front());
                    stash[random_kw[i]].pop();
                }else{
                    if(set_kw.find(random_kw[i])!=set_kw.end())
                        up_op_id.push_back(temp_op_id);
                    else
                        up_op_id.push_back({temp_op_id.first,temp_op_id.second+'*'});
                }
            }
        }        
        

        void gen_random_kw_array(std::vector<std::string>& random_kw,int l){
            for(int i=0;i<l;++i){
                int temp = rand() % fullkw.size();
                random_kw[i]=fullkw[temp];
            }
        }

        void init_MM_st(){
            for(auto kw : fullkw){
                MM_st[kw].key = kw + std::to_string(rand());
                MM_st[kw].query_first = 0;
                MM_st[kw].cnt_true = 0;
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
                MM_st[w].query_first = stoi(out);
                input >> out;
                MM_st[w].cnt_true=stoi(out);
                input >> out;
                MM_st[w].key=out;
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


        void gen_update_list(std::vector<UpdateRequestMessage>& update_list,std::map<std::string,std::string> DX){
            for(auto i : DX){
                UpdateRequestMessage request;
                request.set_l(i.first);
                request.set_e(i.second);
                update_list.push_back(request);
            }
            
        }


        Status update(std::vector<UpdateRequestMessage> update_list) {
            UpdateRequestMessage request;

            ClientContext context;

            ExecuteStatus exec_status;

            std::unique_ptr<ClientWriterInterface<UpdateRequestMessage>> writer(stub_->update(&context, &exec_status));
            int i = 0;		
            while(i < update_list.size()){
                writer->Write(update_list[i]);
                ++i;
            }
            writer->WritesDone();
            Status status = writer->Finish();

            return status;
        }

        void search(const std::string kw,std::map<std::string,std::queue<std::pair<std::string,std::string>>> &stash,std::unordered_set<std::string>& ID){
            std::string x = Util::H_key(MM_st[kw].key,kw); 
            SearchRequestMessage request;
            request.set_cnt(l);
            request.set_q_f(MM_st[kw].query_first);
            request.set_x(x);
            ClientContext context;
            std::unique_ptr<ClientReaderInterface<SearchReply>> reader = stub_->search(&context, request);
           //这里获得了从server端返回的真实且经过op操作以后的id列表。
           //读取返回列表：
            SearchReply reply;
            MM_st[kw].cnt_true = 0;
            std::cout<<"client search"<<std::endl;
            while(reader->Read(&reply)){
                std::string op_id;
                Util::descrypt(K_enc,iv,reply.ind(),op_id);
                std::cout<<op_id<<std::endl;
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
            while(stash[kw].size()!=0){
                std::pair<std::string,std::string> temp = stash[kw].front();;
                if(temp.first == "0")   ID.insert(temp.second);

                else if(temp.second == "1"){
                    if(ID.find(temp.second) != ID.end())  ID.erase(temp.second);
                }
                stash[kw].pop();
            }
            MM_st[kw].key = kw + std::to_string(rand());
            MM_st[kw].cnt_true = ID.size();
            MM_st[kw].query_first = 0;
            re_update(ID,kw);
        }

        void re_update(const std::unordered_set<std::string> ID,const std::string kw){
            std::map<std::string,std::string> DX;
            for(int i=0;i<l-ID.size();i++){
                std::string x = Util::H_key(MM_st[kw].key,kw); //保护关键字
                std::string y = Util::H_key(x,std::to_string(i));  //产生加密索引
                std::string e_value; 
                char op = '2'; //op取值 0：app 1: del 2:full
                Util::encrypt(K_enc,iv,"*"+op,e_value); 
                DX[y] = e_value;
            }
            int i=l-ID.size();
            for(auto j : ID){
                std::string x = Util::H_key(MM_st[kw].key,kw); //保护关键字
                std::string y = Util::H_key(x,std::to_string(i));  //产生加密索引
                std::string e_value; 
                Util::encrypt(K_enc,iv,"0,"+j,e_value);
                DX[y] = e_value;
                i++;
            }
            std::vector<UpdateRequestMessage> update_list;
            gen_update_list(update_list,DX);
            std::cout<<"re_update_list.size = "<<update_list.size()<<std::endl;
            std::cout<<"re_update  data to server"<<std::endl;
            Status status = update(update_list);
            std::cout<<"re_update finished"<<std::endl;
        }         



        private:
            std::map<std::string, st> MM_st; //client存的状态表
            std::vector<std::string> fullkw; //关键字全集
            std::unique_ptr<RPC::Stub> stub_;
            int l; 
            int p;
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