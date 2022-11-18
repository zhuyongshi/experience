#include <iostream>
#include <vector>
#include <time.h>
#include <cmath>
#include <string>
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

using namespace CryptoPP;
using namespace std;

string H1(const string message, string key);

string H(const string message);

int bytesToInt(byte *bytes, int size);

int hash_k_int(const string message, const string key);

string toBinary(int n);

string padding(string s, int len);

int gen_key(byte *key);

string Xor(const string s1, const string s2);

string Permutation(int n, vector<string> kep, string pin);

string De_Permutation(int n, vector<string> kep, string dpin);

vector<string> Find_CK(int n, vector<string> kpa, vector<string> kpb);

int Permutationkey_Gen(string key, int n, vector<string> &ret);

void ANOTH(int ctr, int len, vector<string> m, vector<string> &mplus);

void D_AONTH(int ctr, vector<string> mplus, vector<string> &m);

//随机生成长度为len的01密钥串
string Gen_RandKey(int len);

int Pr_Gen(vector<string> key, vector<string> w, int len, int n,
           vector<string> &P1, vector<string> &P2, vector<string> &P3, string &retkey);

int Pr_Enc(vector<string> key, vector<string> w, vector<string> m, int ctr, int len,
        vector<string> &c);

int Pr_Dec(vector<string> key, vector<string> w, vector<string> c, int ctr, int len,
        vector<string> &m);

int RG(vector<string> key, vector<string> w, int len, int s,
       vector<vector<string>> &RetCK, vector<vector<string>> &RetP2, vector<string> &RetKeyfai);

int RE(vector<vector<string>> CK, vector<vector<string>> P2, vector<string> Keyfai, vector<string> c,
      vector<string> &ret_c);