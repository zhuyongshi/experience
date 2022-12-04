#include<iostream>
#include<string>
#include<bits/stdc++.h>
using namespace std;

int main(){
    unordered_set<string> temp;
    temp.insert("1");
    temp.insert("2");
    temp.insert("3");
    temp.insert("4");
    temp.insert("5");

    for(auto i : temp){
        cout<<i<<endl;
    }
    
    return 0;
}