#include<iostream>
#include<fstream>
#include<bits/stdc++.h>
using namespace std;
int sum = 0;

void get_MM(std::string path){
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
            sum++;
        }
    }

}

int main(){
    std::string union_MM_path = "/home/zws/Desktop/experience/test/union_MM.txt";
    get_MM(union_MM_path);
    cout<<sum<<endl;
    return 0;
}