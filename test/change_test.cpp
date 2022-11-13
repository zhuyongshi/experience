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

// 从关键的全集中随机选关键字，将旧的关键字集合填充到新关键字集
// 这里的l是一个文档包含最大关键词数目
int get_new_kwarr(std::unordered_set<std::string> &old_kwset,std::vector<std::string> &kwarr,int l,int old_length){
    int pad_length = l-old_length;
    int j = old_length;
    for(int i=0;i<pad_length;++i){
        int temp = rand() % fullkw.size();
        while(old_kwset.find(fullkw[temp]) != old_kwset.end()){
            temp = rand() % fullkw.size();
        }
        kwarr[j]=fullkw[temp];
        ++j;
    }
    return 1;
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

int get_l(std::unordered_map<std::string,std::vector<std::string>>& order_MM){
    int max_D_num = 0;
    for(auto order : order_MM){
        int length = order.second.size();
        if(max_D_num<length) max_D_num = length;
    }
    return max_D_num;
}

int main(){
    srand(time(0));
    std::string kw_path = "/home/zws/Desktop/experience/test/key.txt";
    get_w_array(kw_path,fullkw);
    int n;
    int m;
    int l;
    std::unordered_map<std::string,std::vector<std::string>> MM;
    std::string MM_path = "/home/zws/Desktop/experience/test/inverted.txt";
    get_MM(MM_path,MM);
    std::multimap<int,std::string,std::greater<int>> old_result;
    n = get_result(MM,old_result);
    std::cout<<"n="<<n<<std::endl;

    std::ofstream	os("/home/zws/Desktop/experience/test/old_result.txt",std::ios::app);
    for(auto i : old_result){
        os<<i.first<<" "<<i.second<<std::endl;
    }
    os.close();

    for (std::multimap<int, std::string>::iterator it = old_result.begin(); it != old_result.end(); it++){
        os<<it->first<<" "<<it->second<<std::endl;
    }

    std::unordered_map<std::string,std::vector<std::string>> order_MM;
    std::string order_MM_path = "/home/zws/Desktop/experience/test/ans_01";
    get_MM(order_MM_path,order_MM);
    l = get_l(order_MM);
    std::cout<<"l="<<l<<std::endl;


    std::ofstream	os2("/home/zws/Desktop/experience/test/new_order_MM.txt",std::ios::app);
    for(auto i : order_MM){
        std::unordered_set<std::string> old_kwset(i.second.begin(),i.second.end());
        int old_length = i.second.size();
        std::vector<std::string> new_kwarr(l);
        //先把旧的关键字赋给新的关键字集合
        for(int j=0;j<old_length;j++){
            new_kwarr[j] = i.second[j];
        }
        //再增加新的关键字，使得关键字集合一共包含l个元素
        os2<<i.first;
        get_new_kwarr(old_kwset,new_kwarr,l,old_length);
        for(auto j :new_kwarr) os2<<" "<<j;
        os2<<"\n";
    }
    os2.close();

    

    return 0;
}