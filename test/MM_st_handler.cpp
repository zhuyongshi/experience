#include<fstream>
#include<ostream>
#include<iostream>
#include<bits/stdc++.h>
using namespace std;


//若 frist + cnt_true > n 会出现漏掉真实文档的情况，这个比较危险。
struct st{
    int query_first;  //查询头，这个数字也代表着继上一次查询已更新的次数
    int cnt_true; //目前关键字对应的真实文档
    int cnt_up;    //当前对关键字的更新次数
    std::string key;
};

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
    key_myfile.close();
}

//将文件清空
void clear_txt(std::string path){
    ofstream clear;
    clear.open(path,ios::out);
    clear.close();
}

void gen_MM_st(std::string path){
    std::ofstream	os(path,std::ios::app);
    for(auto kw : fullkw){
        os<<kw<<" 0 10 0 miyao"<<"\n";
    }
    os.close();
}

void get_MM(std::string path,std::map<std::string, st>& MM_st){
    std::ifstream MM_myfile(path);
    std::string w,line;
    
    while (getline(MM_myfile, line))								
    {
        std::stringstream input(line);
        std::string out;
        input >> out;
        w = out;
        input >> out;
        MM_st[w].query_first = stoi(out);
        input >> out;
        MM_st[w].cnt_up=stoi(out);
        input >> out;
        MM_st[w].cnt_true=stoi(out);
        input >> out;
        MM_st[w].key=out;
    }
    MM_myfile.close();
}

int main(){
    std::string kw_path = "/home/zws/Desktop/experience/test/01_04/0_key_02.txt";
    get_w_array(kw_path,fullkw);

    string MM_st_path = "/home/zws/Desktop/experience/test/MM_st.txt";
    gen_MM_st(MM_st_path);

    std::map<std::string, st> MM_st;
    get_MM(MM_st_path,MM_st);
    for(auto i : MM_st){
        cout<<i.first<<" "<<i.second.query_first<<" "<<i.second.cnt_up<<" "<<i.second.cnt_true<<" "<<i.second.key<<endl;
    }

    return 0;
}