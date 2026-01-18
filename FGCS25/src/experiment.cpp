#include "../include/experiment.h"
#include <vector>
#include <queue>
#include <map>
#include "../include/Structs.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>    // for std::shuffle
#include <random>       // for std::default_random_engine
#include <chrono>
#include "../include/Block.h"
#include "../include/Query.h"
#include <cmath>


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
        std::string u,v;
        int w;
        
        // 读取每一行中的3个整数
        iss >> u >> v >> w;
        transaction tx(u, v, "friend", w);
        this->transactions.push_back(tx);
    }
    
    file.close();  // 关闭文件
}



void experiment::test_query(std::string filename, int txs_in_one_block, std::vector<int> tw_size,
                            std::string u_q, std::string type_q) {
    std::ifstream file(filename);  // 打开文件
    std::string line;
    std::vector<transaction> transactions;

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
        std::string u,v, type;
        int w;
        
        // 读取每一行中的3个整数
        iss >> u >> v >> type >> w;
        transaction tx(u, v, type, w);
        transactions.push_back(tx);
    }
    file.close();  // 关闭文件

    std::vector<Block> blockchain = Block::construct_chain(transactions, txs_in_one_block, 12*txs_in_one_block, 3, 7);

    // 测试查询
    std::chrono::_V2::system_clock::time_point start, end;
    std::chrono::duration<double> duration;

    for(int tw: tw_size) {
        int lb = 0;
        int ub = tw - 1;

        start = std::chrono::high_resolution_clock::now();
        std::vector<Block> VO = Query::Search(u_q, type_q, lb, ub, blockchain);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        std::cout << "Search Duration: " << duration.count()/50 << " seconds" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        std::vector<transaction> results = Query::Verify(u_q, type_q, lb, ub, blockchain, VO);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        std::cout << "Verify Duration: " << duration.count()/50 << " seconds" << std::endl;

        // 计算VO Size (KB为单位)
        size_t s = 0;
        for(Block& blk : VO) {
            // 固定存储：blk_height, preBkHash
            size_t s_ = 36;
            // 布隆过滤器与摘要的字节数
            s_ = s_ + 32*(blk.e_ + 1) + (blk.e_ + 1) * (int)(std::ceil(blk.bf.m_ / 8));
            // 交易的字节数
            for(transaction& tx : blk.transactions) {
                s_ = s_ + tx.u.length() + tx.v.length() + tx.type.length() + 4;
            }
            s += s_;
        }
        
        std::cout << "VO size: " << s/1024 << " KB" << std::endl;
    }
}