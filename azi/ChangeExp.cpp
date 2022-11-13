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

std::string H1(const std::string message, std::string key)
{
    byte buf[SHA256::DIGESTSIZE];
    std::string salt = key;
    SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
    return std::string((const char *)buf, (size_t)SHA256::DIGESTSIZE);
}

std::string H(const std::string message)
{
    byte buf[SHA256::DIGESTSIZE];
    std::string salt = "01";
    SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
    return std::string((const char *)buf, (size_t)SHA256::DIGESTSIZE);
}

int bytesToInt(byte *bytes, int size = 4)
{
    int addr = bytes[0] & 0xFF;
    addr |= ((bytes[1] << 8) & 0xFF00);
    addr |= ((bytes[2] << 16) & 0xFF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

int hash_k_int(const std::string message, const std::string key)
{
    byte buf[SHA256::DIGESTSIZE];
    std::string salt = key;
    SHA256().CalculateDigest(buf, (CryptoPP::byte *)((message + salt).c_str()), message.length() + salt.length());
    return bytesToInt(buf);
}

std::string toBinary(int n)
{
    std::string r;
    while (n != 0)
    {
        r += (n % 2 == 0 ? "0" : "1");
        n /= 2;
    }
    reverse(r.begin(), r.end());
    return r;
}

std::string padding(std::string s, int len)
{
    std::string r;
    if (s.size() < len)
    {
        int count = len - s.size();
        while (count)
        {
            r += "0";
            count--;
        }
        r += s;
    }
    else
    {
        r = s.substr(0, len);
    }
    return r;
}

int gen_key(byte *key)
{
    //产生一个随机数密钥串，长度为16字节
    AutoSeededRandomPool rand;
    SecByteBlock Key(0x00, AES::DEFAULT_KEYLENGTH);
    rand.GenerateBlock(Key, Key.size());
    key = Key;
    return 1;
}

std::string Xor(const std::string s1, const std::string s2)
{
    // std::cout<< "in len = "<< s1.length()<<", s1 = "<<s1<<std::endl;
    std::string result = s1;
    if (s1.length() != s2.length())
    {
        // ERROR
        std::cout << "not sufficient size: " << s1.length() << ", " << s2.length() << std::endl;
        return "";
    }
    for (int i = 0; i < result.length(); i++)
    {
        result[i] = result[i] ^ s2[i];
    }
    return result;
}

std::string Permutation(int n, std::string kep, std::string pin)
{
    std::string pout;
    for (size_t i = 0; i < n; i++)
    {
        pout += (pin[kep[i] - '0']);
    }
    return pout;
}

std::string Find_CK(int n, std::string kpa, std::string kpb)
{
    int ck[n];
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (kpa[i] == kpb[j])
            {
                ck[j] = i;
                break;
            }
        }
    }
    std::string ret = "";
    for (int i = 0; i < n; i++)
    {
        ret += std::to_string(ck[i]);
    }
    return ret;
}

int Permutationkey_Gen(std::string key, int n, std::vector<std::string> &ret)
{
    std::vector<int> A(n, 0);
    unsigned int tmp, tempo;
    for (int i = 0; i < n; i++)
    {
        A[i] = i;
    }

    for (int j = 0; j < n; j++)
    {
        // std::string a=std::to_string(j);
        std::string iTostr = toBinary(j);
        tmp = hash_k_int(iTostr, key);

        tmp = tmp % n;

        tempo = A[tmp];
        A[tmp] = A[j];
        A[j] = tempo;
    }

    for (int i = 0; i < n; i++)
    {
        ret[i] = std::to_string(A[i]);
    }
    return 0;
}

// srand(time(NULL));
// std::string keyp;  //生成的K'
// int kp;     //K'对应的十进制数
// kp = rand()%(int)(pow(2,length));
// std::string r;   //未填充的二进制串
// int count=0;  //K'转换成二进制时的位数
// while (kp != 0){
//     r += ( kp % 2 == 0 ? '0' : '1' );
//     kp /= 2;
//     count++;
// }
// reverse(r.begin(),r.end());     //将字符串进行反转
// int chance = length-count;
// while (chance!=0)
// {
//     keyp += '0';
//     chance--;
// }
// keyp += r;
// char tmp;
void ANOTH(int ctr, std::vector<std::string> &m, std::vector<std::string> &mplus)
{
    int n = m.size(); // id个数
    std::vector<std::string> x(n);
    std::string allX = "";

    // 随机生成length位01串
    // for (int i = 0; i < length; i++)
    // {
    //     tmp = '0' + rand()%2;
    //     keyp+=tmp;
    // }
    std::string keyp = "10010111";

    // x[i] = m[i] ^ Fk'(ctr+i)
    for (int i = 0; i < n; i++)
    {
        std::string ctrPlus = std::to_string(ctr + i + 1);
        std::string digest = H1(ctrPlus, keyp).substr(0, m[i].size());
        x[i] = Xor(m[i], digest);
        allX += x[i];
    }

    // m'[n+1] = K' ^ H(x[1]...x[n])
    std::string hash_allx = H(allX).substr(0, keyp.size());
    mplus[n] = Xor(keyp, hash_allx); 

    // m'[i] = x[i] ^ H(m'[n+1] ^ (ctr+i))
    for (int i = 0; i < n; i++)
    {
        std::string bina_ctr = toBinary(ctr + i + 1);
        std::string pad_ctr = padding(bina_ctr, mplus[n].size());
        std::string xor_param = H(Xor(mplus[n], pad_ctr)).substr(0, x[i].size());
        mplus[i] = Xor(x[i], xor_param);
    }
    return;
}

void D_AONTH(int ctr, std::vector<std::string> &mplus, std::vector<std::string> &m)
{
    int n = m.size();
    std::vector<std::string> x(n);
    std::string allX = "";
    std::string keyp;

    // x[i] = m'[i] ^ H(m'[n+1] ^ (ctr+i))
    for (int i = 0; i < n; i++)
    {
        std::string bina_ctr = toBinary(ctr + i + 1);
        std::string pad_ctr = padding(bina_ctr, mplus[n].size());
        std::string xor_param = H(Xor(mplus[n], pad_ctr)).substr(0, mplus[i].size()); //
        x[i] = Xor(mplus[i], xor_param);
        allX += x[i];
    }

    // K' = m'[n+1] ^ H(x[1]...x[n])
    std::string hash_allx = H(allX).substr(0, mplus[n].size());
    keyp = Xor(mplus[n], hash_allx);

    // m[i] = x[i] ^ Fk'(ctr+i)
    for (int i = 0; i < n; i++)
    {
        std::string ctrPlus = std::to_string(ctr + i + 1);
        std::string digest = H1(ctrPlus, keyp).substr(0, x[i].size());
        m[i] = Xor(x[i], digest);
    }
    return;
}

int main()
{
    // 测试permutation
    // string kep = "24310";
    // int size = kep.size();
    // string pin = "abcde";
    // string pout = Permutation(size,kep,pin);
    // for (size_t i = 0; i < size; i++)
    // {
    //     cout << pout[i] << " ";
    // }

    //测试 FindConversion Key
    // std::string kpa = "3120";
    // std::string kpb = "2310";
    // int size = kpa.size();
    // std::string ck=Find_CK(size,kpa,kpb);
    // std::cout<<ck<<std::endl;
    //     return 0;

    //测试PGen
    // std::string key="2310";
    // int n=30;
    // std::vector<std::string> kp(n);
    // Permutationkey_Gen(key,n,kp);
    // for(int i=0;i<n;i++){
    //     std::cout<<kp[i]<<' ';
    // }

    //测试AONTH
    int ctr = 20;
    std::vector<std::string> m{"cdg", "efg", "sad", "aqw", "cvb","fhj"};
    std::vector<std::string> mplus(m.size()+1);
    ANOTH(ctr, m, mplus);
    for (int i = 0; i < mplus.size(); i++)
    {
        std::cout << mplus[i] << " ";
    }
    std::cout<<std::endl;

    //測試D-AONTH
    std::vector<std::string> origin_m(mplus.size()-1);
    D_AONTH(ctr, mplus, origin_m);
    for (int i = 0; i < m.size(); i++)
    {
        std::cout << origin_m[i] << " ";
    }
    std::cout<<std::endl;

    return 0;
}