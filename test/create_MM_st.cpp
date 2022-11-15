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

#define AES128_KEY_LEN 16
#define MAX_KW_LEN 8
#define MAX_OP_LEN 1
#define MAX_IND_LEN 16
#define MAX_N_LEN 2 //unsigned int的字节数
#define LABEL_LEN (MAX_KW_LEN + MAX_N_LEN) //label= kw||j
#define VALUE_LEN (MAX_KW_LEN + MAX_N_LEN + MAX_IND_LEN +MAX_OP_LEN) //value=kw||j||id||op

struct st{
    int frist;
    int cnt_up;
    int cnt;
    byte* k;
    byte* ivs;
};

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

//随机产生一个随机数密钥和随机的初始变量。这两个参数都是AES加密需要用到的。
void gen_key_ivs(SecByteBlock& Key,SecByteBlock& ival){
    AutoSeededRandomPool rand;
	rand.GenerateBlock(Key, Key.size());
	rand.GenerateBlock(ival, ival.size());
}

void encrypt(SecByteBlock& Key,SecByteBlock& ival,std::string plaintext,byte* ciphertext){
    CFB_Mode< AES >::Encryption e;
    e.SetKeyWithIV(Key, Key.size(), ival, (size_t)AES::BLOCKSIZE); 
    byte tmp_new_st[AES128_KEY_LEN];
    e.ProcessData(tmp_new_st, (byte*) plaintext.c_str(), plaintext.length());
    memcpy(ciphertext,tmp_new_st,plaintext.length());
}

void gen_plaintext(byte* plaintext,std::string kw,int j){
    
}

std::string padding(const std::string str)
{
    
    size_t BS = (size_t)AES::BLOCKSIZE/2;
    size_t pad_len = BS - str.length() % BS;
    return str + std::string(char(pad_len), pad_len);
}



std::vector<std::string> fullkw;
byte K_ENC[AES128_KEY_LEN];

int main(){
    int n;
    std::map<std::string, st> MM_st;
    std::string kw_path = "/home/zws/Desktop/experience/test/key.txt";
    get_w_array(kw_path,fullkw);
    
    std::string s1 = padding("1001");
    std::cout<<s1.length()<<std::endl;
    
    // for(auto kw : fullkw){
    //     SecByteBlock Key(0x00, AES::DEFAULT_KEYLENGTH);
    //     SecByteBlock ival(AES::BLOCKSIZE);
    //     gen_key_ivs(Key,ival);
    //     for(int i=0;i<n;i++){
    //         byte x[LABEL_LEN];

    //     }
    //     break;
    // }
    return 0;
}