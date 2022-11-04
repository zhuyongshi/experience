#include<iostream>
#include<bitset>
#include<bits/stdc++.h>

#include <crypto++/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include<cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>
#include<cryptopp/files.h>
using namespace CryptoPP;

std::vector<std::string> fullkw;

void get_w_array(std::string path, std::vector<std::string>& w_array){
    std::ifstream key_myfile(path);
    std::string w;
    while (getline(key_myfile, w))								
	{
		std::stringstream input(w);
		std::string out;
		while (input >> out) {
			w_array.push_back(out);
		}
	}
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
}

 
//从关键的全集中随机选关键字，将旧的关键字集合填充到新关键字集
//这里的l是一个文档包含最大关键词数目
int get_new_kwset(std::unordered_set<std::string> &kwset,std::vector<std::string> &kwarr,int l){
    int pad_length = l-kwarr.size();
    for(int i=0;i<pad_length;++i){
        int temp = rand() % fullkw.size();
        while(kwset.find(fullkw[temp]) != kwset.end()){
            temp = rand() % fullkw.size();
        }
        kwarr.push_back(fullkw[temp]);
    }
    return 1;
}


struct key_st
{
   int first;
   int update_num;
   byte* key;
};

int gen_key(byte* key){
    //产生一个随机数密钥串，长度为16字节
    AutoSeededRandomPool rand;
    SecByteBlock Key(0x00, AES::DEFAULT_KEYLENGTH);
    rand.GenerateBlock(Key, Key.size());
    return 1;
}

int init_MM_st(key_st* MM_st,std::unordered_map<std::string,int>& kwToMM_st){
    for(int i=0;i<fullkw.size();i++){
        MM_st[i].first = 0;
        MM_st[i].update_num = 0;
        gen_key(MM_st[i].key);
    }
    int j=0;
	for(auto i:fullkw){
        kwToMM_st.emplace(i,j);
        ++j;
    }
    return 1;
}

int get_old_set(std::unordered_map<std::string,std::vector<std::string>>& MM,std::map<int,std::string>& old_result){
    int max_D_num = 0;
    for(auto kw : fullkw){
        int length = MM[kw].size();
        old_result.emplace(length,kw);
        if(max_D_num<length) max_D_num = length;
    }
    return max_D_num;
}

int get_l(std::unordered_map<std::string,std::vector<std::string>>& order_MM){
    int max_D_num = 0;
    for(auto order : order_MM){
        int length = order.second.size();
        if(max_D_num<length) max_D_num = length;
    }
    return max_D_num;
}

int main(){
    // key_st MM_st[fullkw.size()];
    // std::unordered_map<std::string,int> kwToMM_st;
    // init_MM_st(MM_st,kwToMM_st);

    std::string kw_path = "/home/zws/Desktop/experience/test/key_01.txt";
    get_w_array(kw_path,fullkw);
    int n;
    int m;
    int l;
    std::unordered_map<std::string,std::vector<std::string>> MM;
    std::string MM_path = "/home/zws/Desktop/experience/test/inverted_01.txt";
    get_MM(MM_path,MM);
    std::map<int,std::string> old_result;
    n = get_old_set(MM,old_result);
    std::cout<<n<<std::endl;
    

    std::unordered_map<std::string,std::vector<std::string>> order_MM;
    std::string order_MM_path = "/home/zws/Desktop/experience/test/ans_01";
    get_MM(order_MM_path,order_MM);
    l = get_l(order_MM);
    std::cout<<l<<std::endl;
    
    return 0;
}