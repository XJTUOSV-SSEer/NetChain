#ifndef PROCESS_PAYSIM_HPP
#define PROCESS_PAYSIM_HPP

#include <vector>
#include <queue>
#include <map>
#include "../base/include/Structs.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>    // for std::shuffle
#include <random>       // for std::default_random_engine


class process_paysim{
public:
    /*
        将paysim数据集中的账户交易信息转换为交易，并储存在target_file中
        input:
            seg_size - 对相同复合键聚合后的segment size
    */
    static void process_paysim_dataset(std::string filename, std::string target_file, size_t seg_size) {
        std::vector<transaction> transactions;
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "无法打开文件: " << filename << std::endl;
            return;
        }

        std::string line;
        // 跳过表头
        std::getline(file, line);

        std::set<std::string> account_set;         // 储存出现的账户名

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string field;
            std::vector<std::string> fields;

            // 按逗号分割字段（简单 CSV 解析，假设无引号或换行）
            while (std::getline(ss, field, ',')) {
                fields.push_back(field);
            }

            // PaySim 应有 11 列
            if (fields.size() < 11) {
                std::cerr << "警告：跳过格式错误的行: " << line << std::endl;
                continue;
            }

            std::string type = fields[1];          // type
            double amount = 0;
            try {
                amount = std::stod(fields[2]);  // amount
            } catch (const std::exception& e) {
                std::cerr << "无效金额: " << fields[2] << std::endl;
                continue;
            }
            std::string nameOrig = fields[3];      // nameOrig
            std::string nameDest = fields[6];      // nameDest
            transactions.push_back(transaction(nameOrig, nameDest, type, (int)amount));
            account_set.insert(nameOrig);
            account_set.insert(nameDest);
        }

        file.close();

        // 为复合键<C1231006815, PAYMENT>再生成2500条随机交易
        std::vector<std::string> account_list(account_set.begin(), account_set.end());
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis1(0, static_cast<int>(account_list.size()) - 1);
        std::uniform_int_distribution<int> dis2(5000, 500000);
        for(size_t i = 0; i<2500; i++) {
            std::string nameDest = account_list[dis1(gen)];     // 随机选取一个接收账户
            int amount = dis2(gen);
            // 构造一条随机交易
            std::string nameOrig = "C1231006815";
            std::string type = "PAYMENT";
            transactions.push_back(transaction(nameOrig, nameDest, type, (int)amount));
        }

        std::cout << transactions.size() << std::endl;

        // 根据复合键对交易分segment，然后shuffle
        std::map<std::string, std::queue<transaction>> multimap;       // 储存复合键->对应交易
        for(transaction& tx : transactions){
            std::string kw = tx.u + tx.type;
            if(multimap.find(kw)==multimap.end()){
                multimap[kw] = std::queue<transaction>();
            }
            multimap[kw].push(tx);
        }
        std::vector<std::vector<transaction>> segments;
        for(std::map<std::string, std::queue<transaction>>::iterator it = multimap.begin(); it!=multimap.end(); it++){
            std::string kw = it -> first;
            std::queue<transaction> txs = it->second;
            while(!txs.empty()) {
                std::vector<transaction> segment;
                for(size_t i=0; i<seg_size; i++) {
                    segment.push_back(txs.front());
                    txs.pop();
                    // 若当前txs内元素不足seg_size且集合已空，提前退出
                    if(txs.empty()){
                        break;
                    }
                }
                // 将segment加入segments
                segments.push_back(segment);
            }
        }

        // 对segments进行shuffle
        std::default_random_engine rng(65537);
        std::shuffle(segments.begin(), segments.end(), rng);


        // 将交易写入文件        
        std::ofstream outFile(target_file);     // 打开输出文件流
        if (!outFile) {
            std::cout << "Error opening file for writing" << std::endl;
            return;
        }
        for(std::vector<transaction>& segment : segments) {
            for(transaction& tx : segment) {
                outFile << tx.u << ' '<< tx.v << ' ' << tx.type << ' ' << tx.w << '\n';
            }
        }
        outFile.close();
    }




    /*
        对paysim数据集进行统计分析，寻找对应交易数最多的复合键
    */
    static void stat_paysim_dataset(std::string filename) {
        std::ifstream file(filename);  // 打开文件
        std::string line;
        std::map<std::string, int> m;

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
            std::string u, v, type;
            int w;
            
            // 读取每一行中的两个整数
            iss >> u >> v >> type >> w;
            std::string key = u + type;
            if(m.find(key)==m.end()){
                m[key] = 0;
            }
            m[key] += 1;
        }
        
        file.close();  // 关闭文件

        // 将统计信息写入文件
        std::ofstream outputfile("../../dataset/stat.txt");

        for(std::map<std::string, int>::iterator it = m.begin(); it!=m.end(); it++){
            outputfile << it->first << " " << it->second <<std::endl;
        }

        // 寻找对应最多交易的复合键
        int max_amount = 0;
        std::string max_key;
        for(std::map<std::string, int>::iterator it = m.begin(); it!=m.end(); it++) {
            if(it->second > max_amount){
                max_key = it->first;
                max_amount = it->second;
            }
        }
        std::cout << max_key << " " << max_amount << std::endl;

        return;
    }
};






#endif