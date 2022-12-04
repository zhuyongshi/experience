#include <iostream>
#include <time.h>
#include <cmath>
#include <bits/stdc++.h>
#include <crypto++/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include "ecsse_util.h"

using namespace CryptoPP;

#define ANOTHKEY "ANOTH"

//从word中获取w1和w2
int find_w12_from_w(std::string word, std::string &w1, std::string &w2);

// 排列加密算法: 将pin按kep顺序排列，pin为string
std::string Permutation(int n, std::vector<int> kep, std::string pin);

// 排列加密算法: 将pin按kep顺序重排回原列
std::string De_Permutation(int n, std::vector<int> kep, std::string dpin);

// 排列加密算法：将pin按kep顺序排列，pin为string数组
int Permutation2(int start, std::vector<int> kep, std::vector<std::string> pin, std::vector<std::string> &dpin);

// 排列加密算法: 将pin按kep顺序重排回原列
int De_Permutation2(int start, std::vector<int> kep, std::vector<std::string> dpin, std::vector<std::string> &pin);

// 根据kpa和kpb调整顺序
std::vector<int> Find_CK(int n, std::vector<int> kpa, std::vector<int> kpb);

// 密钥扩展函数，将密钥从key长度扩展到n长度
int Permutationkey_Gen(std::string key, int n, std::vector<int> &ret);

int ANOTH(int len, std::vector<std::string> m, std::vector<std::string> &mplus);

int D_AONTH(std::vector<std::string> mplus, std::vector<std::string> &m);

int Pr_Gen(std::vector<std::string> key, std::vector<std::string> w, int len, int n, std::vector<std::string> &P1, std::vector<std::string> &P2, std::vector<std::string> &P3, std::string &retkey);

int Pr_Enc(std::vector<std::string> key, std::vector<std::string> w, std::vector<std::string> m, int len, std::vector<std::string> &c);

int Pr_Dec(std::vector<std::string> key, std::vector<std::string> w, std::vector<std::string> c, int len, std::vector<std::string> &m);

int Pr_ReGen(std::vector<std::string> key, std::vector<std::string> w, int len, int s, std::vector<std::vector<int>> &RetCK, std::vector<std::vector<int>> &RetP2, std::vector<std::string> &RetKeyPhi);

int Pr_ReEnc(std::vector<std::vector<int>> CK, std::vector<std::vector<int>> P2, std::vector<std::string> KeyPhi, std::vector<std::string> c, std::vector<std::string> &ret_c);

void sEMM_Setup(int lambda, std::map<std::string, std::vector<std::string>> MMp, std::string &mskp, std::map<std::string, std::vector<std::string>> &EMMp);

void sEMM_Token(std::string mskp, std::string w1, std::string w2, std::string &token);

void sEMM_Search(std::string tokp, std::map<std::string, std::vector<std::string>> EMMp, std::vector<std::string> &tags);

typedef struct
{
    std::string kx;
    std::vector<std::vector<int>> CK;
    std::vector<std::vector<int>> P2;
    std::vector<std::string> KeyPhi;
} key_re_d;

typedef struct
{
    std::string kv;
    std::string kt;
    std::string kp;
    std::string kx;
    std::string kxor;
    std::string mskp;
} MK;

typedef struct
{
    std::map<std::string, std::vector<std::string>> EMMp;
    std::multiset<std::string> Xset;
} EMM;

typedef struct
{
    int lambda;
    int mu;
    std::map<std::string, std::vector<std::string>> MM;
} pr_filter_setup_param;

typedef struct
{
    MK mk;
    EMM emm;
} pr_filter_setup_res;

int PR_Filter_Setup(pr_filter_setup_param param, pr_filter_setup_res &res);

typedef struct
{
    MK mk;
    std::vector<std::string> words;
    int len;
    int s;
} pr_filter_token_param;

typedef struct
{
    std::string tokp;
    std::string k_w12_enc;
    std::vector<key_re_d> tokp_vec;
} pr_filter_token_res;

int PR_Filter_Token(pr_filter_token_param param, pr_filter_token_res &res);

typedef struct
{
    pr_filter_token_res tokq;
    EMM emm;
} pr_filter_search_param;

typedef struct
{
    std::vector<std::string> c;
    std::vector<bool> vaild;
} pr_filter_search_res;

int PR_Filter_Search(pr_filter_search_param param, pr_filter_search_res &res);

typedef struct
{
    std::string w1;
    std::string wn;
    MK mk;
    std::vector<std::string> c;
    std::vector<bool> vaild;
} pr_filter_resolve_param;

int PR_Filter_Resolve(pr_filter_resolve_param param, std::vector<std::string> &res);