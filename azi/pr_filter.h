#include <iostream>
#include <time.h>
#include <cmath>
#include <bits/stdc++.h>
// #include <std::string>
// #include <std::vector>

#include <crypto++/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>

using namespace CryptoPP;

// hash函数 带盐值
std::string H1(const std::string message, std::string key);

// hash函数，不带盐值
std::string H(const std::string message);

// hash函数，输出Int
int hash_k_int(const std::string message, const std::string key);

// byte转Int
int bytesToInt(byte *bytes, int size);

// Int转byte
byte *IntToBytes(int num);

// 整数转二进制
std::string toBinary(int n);

// 填充算法: 后面填0
std::string padding(std::string s, int len);

// 产生随机数密钥串，类型为byte
int gen_key(byte *key1);

// 产生随机数密钥串，类型为string
std::string Gen_RandKey(int len);

// string 异或
std::string Xor(const std::string s1, const std::string s2);

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

int ANOTH(int ctr, int len, std::vector<std::string> m, std::vector<std::string> &mplus);

int D_AONTH(int ctr, std::vector<std::string> mplus, std::vector<std::string> &m);

int Pr_Gen(std::vector<std::string> key, std::vector<std::string> w, int len, int n,
           std::vector<std::string> &P1, std::vector<std::string> &P2, std::vector<std::string> &P3, std::string &retkey);

int Pr_Enc(std::vector<std::string> key, std::vector<std::string> w, std::vector<std::string> m, int ctr, int len,
           std::vector<std::string> &c);

int Pr_Dec(std::vector<std::string> key, std::vector<std::string> w, std::vector<std::string> c, int ctr, int len,
           std::vector<std::string> &m);

int Pr_ReGen(std::vector<std::string> key, std::vector<std::string> w, int len, int s,
             std::vector<std::vector<int>> &RetCK, std::vector<std::vector<int>> &RetP2, std::vector<std::string> &RetKeyPhi);

int Pr_ReEnc(std::vector<std::vector<int>> CK, std::vector<std::vector<int>> P2, std::vector<std::string> KeyPhi, std::vector<std::string> c,
             std::vector<std::string> &ret_c);