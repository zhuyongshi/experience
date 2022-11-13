#include<iostream>
#include<fstream>
#include<bits/stdc++.h>
using namespace std;

std::vector<std::string> fullkw;

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

int get_result(std::unordered_map<std::string,std::vector<std::string>>& MM,std::multimap<int,std::string,std::greater<int>>& old_result){
    int max_D_num = 0;
    for(auto kw : fullkw){
        int length = MM[kw].size();
        old_result.emplace(length,kw);
        if(max_D_num<length) max_D_num = length;
    }
    return max_D_num;
}

void get_MM(std::string path,std::unordered_map<std::string,std::vector<std::string>>& MM){
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
		}
        MM[w] = ids;
	}
}

int main(){
    std::string kw_path = "/home/zws/Desktop/experience/test/key_01.txt";
    get_w_array(kw_path,fullkw);

    int n;
    int m;
    int l;
    std::unordered_map<std::string,std::vector<std::string>> MM;
    std::string MM_path = "/home/zws/Desktop/experience/test/new_inverted_01.txt";
    get_MM(MM_path,MM);
    std::multimap<int,std::string,std::greater<int>> new_result;
    n = get_result(MM,new_result);
    std::cout<<n<<std::endl;

    std::ofstream	os("/home/zws/Desktop/experience/test/new_result.txt",std::ios::app);
    for(auto i : new_result){
        os<<i.first<<" "<<i.second<<std::endl;
    }
    os.close();

    return 0;
}