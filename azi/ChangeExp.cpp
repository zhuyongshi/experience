#include <iostream>
#include <vector>
#include <time.h>
#include <cmath>
#include <string>
#include<bits/stdc++.h>


using namespace std;


std::vector<int> Permutation(int n,int kep[],int pin[]){
        std::vector<int> pout;

        for (int i = 0; i < n; i++)
        {
            pout.push_back(pin[kep[i]]);
        }
        
        return pout;
    }

int Find_CK(int n,int kpa[],int kpb[],vector<int> &ck){

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
        return 1;
    }

vector<int> Permutatiuonkey_Gen(string key[],int n){
    vector<int> A(n,0);
    int tmp,tempo;
    for (int i = 0; i < n; i++)
    {
        A[i] = i;
    }
    string ciphertext;
    for (int j = 0; j < n; j++)
    {
        // hash函数 tmp = H1(i,key); 
        tempo = A[tmp];
        A[tmp] = A[i]; 
        A[i] = tempo;
    }
    return A;
}

vector<string> ANOTH(int ctr,string m[],int length){
    //随机生成length位01串
    srand(time(NULL));
    string keyp;  //生成的K'
    int kp;     //K'对应的十进制数
    kp = rand()%(pow(2,length));
    string r;   //未填充的二进制串
    int count=0;  //K'转换成二进制时的位数
    while (kp != 0){
        r += ( kp % 2 == 0 ? '0' : '1' );
        kp /= 2;
        count++;
    }
    reverse(r.begin(),r.end());     //将字符串进行反转
    int chance = length-count;
    while (chance!=0)
    {
        keyp += '0';
        chance--;
    }
    keyp += r;

    char tmp;
    int size = sizeof(m);   //id个数
    for (int i = 0; i < length; i++)
    {
        tmp = '0' + rand()%2;
        keyp+=tmp;
    }

    //构造串X
    string ciphertext;
    string x;
    for (int j = 0; j < size; j++)
    {
        encrypt(keyp,ctr+j,&ciphertext);
        x += (Xor(m[j],ciphertext));
    }

    vector<string> mplus[size+2];
    string keyx;    //串x的hash返回值
    string keym[size+1];    //串m'的hash返回值 
    mplus[size] = Xor(keyp,H1(x,keyx));
    for (int k = 0; k < size; k++)
    {
        mplus[k] = (Xor(x[k],H1(Xor(mplus[size],(ctr+k)),keym));
    }
    mplus->push_back('\0');
    return mplus;
}



int main(){
    /* 测试permutation
    int kep[10] = {0,1,2,3,4,5,6,7,8,9};
    int size = 10;
    int pin[10] = {1,4,6,8,0,11,19,20,44,11};
    vector<int> pout = Permutation(size,kep,pin);
    for (int i = 0; i < 10; i++)
    {
        cout << pout[i] << " ";
    }
    */
   /*测试 FindConversion Key
   int size = 10;
   int kpa[10] = {0,1,2,3,4,5,6,7,8,9};
   int kpb[10] = {9,8,7,6,5,4,3,2,1,0};
   vector<int> ck(10,0);
   Find_CK(size,kpa,kpb,ck);
   if (Find_CK(size,kpa,kpb,ck) == 1)
   {
    cout << "ok" << endl;
   }else cout << "fail" << endl;
   
   for (int j = 0; j < 10; j++)
   {
        cout << ck[j] << " ";
   }
   */
}