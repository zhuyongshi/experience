#include<iostream>
#include<fstream>
#include<bits/stdc++.h>
using namespace std;

std::vector<std::string> fullkw;
std::map<std::string,std::queue<std::string>> stash;
std::map<std::string,int> stash_id; //记录stash里面需要存多少个文档
int stash_number; //记录stash里面有多少个对

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

void get_MM(std::string path,std::unordered_map<std::string,std::vector<std::string>>& MM){
     std::ifstream MM_myfile(path);
     std::string w,line;
     
     while (getline(MM_myfile, line))								
	{
		std::stringstream input(line);
		std::string out;
        std::vector<std::string> ids;
        input >> out;
        w = out;
		while (input >> out) {
			ids.push_back(out);
		}
        MM[w] = ids;
	}
    MM_myfile.close();
}

int get_l(std::unordered_map<std::string,std::vector<std::string>>& order_MM){
    int max_D_num = 0;
    for(auto order : order_MM){
        int length = order.second.size();
        if(max_D_num<length) max_D_num = length;
    }
    return max_D_num;
}

void gen_random_kw_array(std::vector<std::string>& random_kw,int l){
    for(int i=0;i<l;++i){
        int temp = rand() % fullkw.size();
        random_kw.push_back(fullkw[temp]);
    }
}

void write_stash(std::vector<std::string>& random_kw,std::vector<std::string>& kw,std::string id){
    unordered_set<std::string> set_random_kw(random_kw.begin(),random_kw.end());
    unordered_set<std::string> set_kw(kw.begin(),kw.end());
    //kw中的元素没在抽样列表里面就放进stash里面
    stash_id[id] = 0;
    for(int i=0;i<kw.size();i++){
        if(set_random_kw.find(kw[i]) == set_random_kw.end()){
            if(stash.find(kw[i])==stash.end()){
                std::queue<std::string> temp;
                temp.push(id);
                stash[kw[i]] = temp;
            }else{
                stash[kw[i]].push(id);
            }
            ++stash_number; 
            ++stash_id[id];    
        }
    }
    
    //在抽样列表里面，不在真kw里面，又在stash里面，则从stash里面拿出来
    for(int i=0;i<random_kw.size();i++){
        if(stash.find(random_kw[i])!=stash.end() && set_kw.find(random_kw[i])==set_kw.end()){
            //控制队列不为空
            if(stash[random_kw[i]].size()>0){
                string s_id = stash[random_kw[i]].front();
                stash[random_kw[i]].pop();
                if(stash_id[s_id]>0){
                    --stash_id[s_id];
                    if(stash_id[s_id]==0) stash_id.erase(s_id);
                }
                --stash_number;
            } 
        }
    }
}



int main(){
    int update_pair_num = 0;
    srand(time(0));
    std::string kw_path = "/home/zws/Desktop/experience/test/01_04/0_key_02.txt";
    get_w_array(kw_path,fullkw);
    stash_number = 0;

    int l = 0;
    std::unordered_map<std::string,std::vector<std::string>> order_MM;
    std::string order_MM_path = "/home/zws/Desktop/experience/test/01_04/0_ans";
    get_MM(order_MM_path,order_MM);
    l = get_l(order_MM);
    l = 0.06 * fullkw.size();
    std::cout<<"l="<<l<<std::endl;

    int sum = 0;
    for(auto i : order_MM){
        std::vector<std::string> random_kw(l,"");
        gen_random_kw_array(random_kw,l);
        write_stash(random_kw,i.second,i.first);
        cout<<stash_id.size()<<" ";
        cout<<stash_number<<endl;
        update_pair_num += i.second.size(); 
        sum++;
        if(sum == 100) break;
    }


    cout<<"update_pair_num = "<<update_pair_num<<endl;
    return 0;
}