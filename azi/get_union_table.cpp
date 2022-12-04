#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
using namespace std;

void get_MM(std::string path, std::map<std::string, std::vector<std::string>> &MM)
{
    std::ifstream MM_myfile(path);
    std::string w, line;
    while (getline(MM_myfile, line))
    {
        std::stringstream input(line);
        std::string out;
        std::vector<std::string> ids;
        input >> out;
        w = out;
        while (input >> out)
        {
            ids.push_back(out);
        }
        MM[w] = ids;
    }
}

int main()
{
    std::map<std::string, std::vector<std::string>> inverted_MM;
    std::string inverted_MM_path = "/home/zws/Desktop/experience/test/inverted.txt";
    get_MM(inverted_MM_path, inverted_MM);

    int sum = 0;

    std::ofstream	os("/home/zws/Desktop/experience/test/union_MM.txt",std::ios::app);
    std::map<std::string,std::vector<std::string>>::iterator i = inverted_MM.begin();
    bool flag = true;
    for(;i!=inverted_MM.end();i++){
        std::unordered_set<std::string> k1(i->second.begin(),i->second.end());
        std::map<std::string,std::vector<std::string>>::iterator j = i;
        ++j;
        cout<<j->first<<endl;
        for(;j!=inverted_MM.end();j++){
            for(auto z : j->second){
                if(k1.find(z)!=k1.end() && flag){
                    os<<i->first<<"∩"<<j->first<<" "<<z<<" ";
                    flag = false;
                } 
                else if(k1.find(z)!=k1.end() && !flag){
                    os<<z<<" ";
                }
            }
            if(flag==false) os<<"\n";
            flag = true;
        }
        sum++;
        if(sum==2) break;
    }
    os.close();
    return 0;
}