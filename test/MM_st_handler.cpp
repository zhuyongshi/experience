#include<fstream>
#include<ostream>
#include<iostream>
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
byte k[17] = "0123456789abcdef";
std::string iv = "0123456789abcdef";

//若 frist + cnt_true > n 会出现漏掉真实文档的情况，这个比较危险。
struct st{
    int query_first;  //查询头，这个数字也代表着继上一次查询已更新的次数
    int cnt_true; //目前关键字对应的真实文档
    int cnt_up;    //当前对关键字的更新次数
    std::string key;
};

std::vector<std::string> fullkw;

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

//将文件清空
void clear_txt(std::string path){
    std::ofstream clear;
    clear.open(path,std::ios::out);
    clear.close();
}

std::string padding(const std::string str)
    {
        size_t BS = (size_t)AES::BLOCKSIZE;
        size_t pad_len = BS - str.length() % BS;
        return str + std::string(char(pad_len), pad_len);
    }

std::string gen_kw_key(const std::string kw){
    // 使用padding方式将所有字符串补齐到16的整数倍长度
    std::string kw_padding;
    std::string kw_key;
    try
    {
        CFB_Mode< AES >::Encryption e;
       e.SetKeyWithIV(k, 16, (byte*)iv.c_str(), (size_t)AES::BLOCKSIZE);
        kw_padding = padding(kw);
        byte tmp_new_st[16];
        e.ProcessData(tmp_new_st, (byte*) kw_padding.c_str(), 16);
        kw_key= std::string((const char*)tmp_new_st, 16);
    }
    catch(const CryptoPP::Exception& e)
    {
        std::cerr << "in gen_kw_key() 在生成kw_key中出问题 " << e.what()<< std::endl;
        exit(1);
    }
    return kw_key;
}

void init_MM_st(std::string path,std::map<std::string, st>& MM_st){
    std::ofstream	os(path,std::ios::app);
    for(auto kw : fullkw){
        std::string kw_key = gen_kw_key(kw);
        MM_st[kw].key = "kw_key";
        MM_st[kw].query_first = 0;
        MM_st[kw].cnt_true = 0;
        MM_st[kw].cnt_up = 9;
        os<<kw<<" "<<MM_st[kw].query_first<<" "<<MM_st[kw].cnt_up<<" "<<MM_st[kw].cnt_true<<" "<<MM_st[kw].key<<"\n";
        
    }
    os.close();
}



void get_MM_st(std::string path,std::map<std::string, st>& MM_st){
    std::ifstream MM_myfile(path);
    std::string w,line;
    
    while (getline(MM_myfile, line))								
    {
        std::stringstream input(line);
        std::string out;
        input >> out;
        w = out;
        input >> out;
        MM_st[w].query_first = stoi(out);
        input >> out;
        MM_st[w].cnt_up=stoi(out);
        input >> out;
        MM_st[w].cnt_true=stoi(out);
        input >> out;
        MM_st[w].key=out;
    }
    MM_myfile.close();
}

bool file_exist (const std::string& path) {
    return ( access( path.c_str(), F_OK ) != -1 );
}

int main(){
    std::string kw_path = "/home/zws/Desktop/experience/test/01_04/0_key_02.txt";
    get_w_array(kw_path,fullkw);

    std::map<std::string, st> MM_st;
    std::string MM_st_path = "/home/zws/Desktop/experience/test/MM_st.txt";
    init_MM_st(MM_st_path,MM_st);

    
    get_MM_st(MM_st_path,MM_st);
    for(auto i : MM_st){
        std::cout<<i.first<<" "<<i.second.query_first<<" "<<i.second.cnt_up<<" "<<i.second.cnt_true<<" "<<i.second.key<<std::endl;
    }

    // //test file_exist function
    // cout<<file_exist(kw_path)<<endl;
    // cout<<file_exist(MM_st_path)<<endl;

    return 0;
}