/*
 * Created by yongshu zhu  on 12/11/2022
 * Email: 993015403@qq.com
 * 
 */

#include "VH.client.h"
#include "VH.Util.h"
#include <cmath>
#include <chrono> // 是一个time library ，源于boost库，已经是c++标准了
#include <bits/stdc++.h>

namespace VH{
    //从文件中取出顺序表的方法
    void get_MM(std::string path,std::unordered_map<std::string,std::vector<std::string>>& MM){
        std::ifstream MM_myfile(path);
        std::string id,line;
        while (getline(MM_myfile, line))								
        {
            std::stringstream input(line);
            std::string out;
            std::vector<std::string> ws;
            input >> out;
            id = out;
            while (input >> out) {
                ws.push_back(out);
            }
            MM[id] = ws;
        }
    }

    void setup(Client &client){
        std::string order_MM_path ="/home/zws/Desktop/experience/test/01_04/0_ans";
        std::unordered_map<std::string,std::vector<std::string>> order_MM;
        get_MM(order_MM_path,order_MM);
    }

}