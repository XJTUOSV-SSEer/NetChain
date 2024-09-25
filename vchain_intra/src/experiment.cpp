#include "../include/experiment.h"


#include <vector>
#include <queue>
#include <map>
#include "Structs.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>    // for std::shuffle
#include <random>       // for std::default_random_engine


void experiment::show_dataset(std::string filename, int num){
    std::ifstream file(filename);  // 打开文件
    std::string line;
    std::map<int, int> m;

    if (!file.is_open()) {
        std::cout << "can not open file" << std::endl;
        return;
    }

    // 逐行读取文件
    while (std::getline(file, line)) {
        // 跳过以 # 开头的行
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        int a, b;
        
        // 读取每一行中的两个整数
        iss >> a >> b;
        if(m.find(a)==m.end()){
            m[a] = 0;
        }
        m[a] = m[a]+1;
    }
    
    file.close();  // 关闭文件


    for(std::map<int, int>::iterator it = m.begin(); it!=m.end(); it++){
        std::cout<< it->first << " " << it->second <<std::endl;
    }

    return;
}



void experiment::adjust_dataset(std::string filename, int seg_size, int ub, std::string target_file){
    std::ifstream file(filename);  // 打开文件
    std::string line;
    // 键为u，值为相应的v的集合
    std::map<int, std::queue<int>> multimap;

    if (!file.is_open()) {
        std::cout << "can not open file" << std::endl;
        return;
    }

    // 逐行读取文件
    while (std::getline(file, line)) {
        // 跳过以 # 开头的行
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        int a, b;        
        // 读取每一行中的两个整数
        iss >> a >> b;

        if(multimap.find(a)==multimap.end()){
            multimap[a] = std::queue<int>();
        }
        multimap[a].push(b);
    }

    // 将multimap中每个u对应的v进行分段
    std::vector<segment> segments;
    for(std::map<int, std::queue<int>>::iterator it = multimap.begin(); it!=multimap.end(); it++){
        int u = it -> first;
        std::queue<int> v_set = it->second;

        
        while(!v_set.empty()){
            segment s;
            for(int i=0; i<seg_size; i++){
                s.v.push_back(std::make_pair(u, v_set.front()));
                v_set.pop();

                // 若当前s内元素不足seg_size且集合已空，提前退出
                if(v_set.empty()){
                    break;
                }
            }
            // 将一个segment加入segments
            segments.push_back(s);
        }
    }

    // 对以上segment进行shuffle
    std::default_random_engine rng(65537);
    std::shuffle(segments.begin(), segments.end(), rng);

    file.close();  // 关闭文件
    // 打开输出文件流
    std::ofstream outFile(target_file);

    // 将shuffle后的结果依次写入文件，且加入随机的权重
    for(int i=0; i< segments.size(); i++){
        segment& seg = segments[i];
        for(int j=0; j<seg.v.size(); j++){
            std::pair<int, int> p = seg.v[j];
            // 随机的w
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, ub);
            int w = dist(gen);

            // 将u,v,w写入文件
            
            if (!outFile) {
                std::cout << "Error opening file for writing" << std::endl;
                return;
            }
            // 写入
            outFile << p.first << ' '<< p.second << ' ' << w<< '\n';
        }
    }
    
    
    outFile.close();
}





experiment::experiment(std::string filename){
    std::ifstream file(filename);  // 打开文件
    std::string line;

    if (!file.is_open()) {
        std::cout << "can not open file" << std::endl;
        return;
    }

    // 逐行读取文件
    while (std::getline(file, line)) {
        // 跳过以 # 开头的行
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        int u, v, w;
        
        // 读取每一行中的3个整数
        iss >> u >> v >> w;
        transaction tx(std::to_string(u), std::to_string(v), "friend", w);
        this->transactions.push_back(tx);
    }
    
    file.close();  // 关闭文件
}