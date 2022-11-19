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
#include<cryptopp/config_int.h>
using namespace CryptoPP;


    
int bytesToInt(byte *bytes, int size = 4)
{
    int addr = bytes[0] & 0xFF;
    addr |= ((bytes[1] << 8) & 0xFF00);
    addr |= ((bytes[2] << 16) & 0xFF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}
int Permutation2(int n, std::vector<std::string> kep, std::vector<std::string> pin, std::vector<std::string> &pout)
{

    for (size_t i = 0; i < n; i++)
    {
        pout[i] = (pin[atoi(kep[i].c_str())]);
    }
    return 0;
}

int main(){
    int n=5;
    std::vector<std::string> kep{"4","2","1","3","0"};
    std::vector<std::string> pin{"id4","id3","id1","id6","id8"};
    std::vector<std::string> pout(n);
    Permutation2(n, kep, pin, pout);
    for(int i = 0; i < n; i++){
        std::cout<<pout[i]<<std::endl;
    }   
    return 0;
}