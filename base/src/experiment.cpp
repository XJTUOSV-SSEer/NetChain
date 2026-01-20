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



std::vector<transaction> experiment::get_txs(std::string filename) {
    std::ifstream file(filename);  // 打开文件
    std::string line;
    std::vector<transaction> transactions;

    if (!file.is_open()) {
        std::cout << "can not open file" << std::endl;
        return transactions;
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
        
        // 读取每一行中的3个整数
        iss >> u >> v >> type >> w;
        transaction tx(u, v, type, w);
        transactions.push_back(tx);
    }
    file.close();  // 关闭文件
    return transactions;
}









void experiment::test_mining(std::string filename, int txs_in_one_block){
    std::vector<transaction> transactions = get_txs(filename);
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Block> blockchain = Block::construct_chain(transactions, txs_in_one_block);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    // 输出执行时间
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    // 输出每块的平均构造时间
    std::cout << "Duration: " << duration.count() / ((double)blockchain.size()) << " seconds" << std::endl;

    // 区块中ADS的平均大小/KB
    long long total_size = 0;
    for(size_t i=0; i<blockchain.size(); i++){
        total_size = total_size + test_ADS_size(blockchain[i]);
    }
    std::cout << "ADS SIZE: " << (double)total_size/(double)(blockchain.size()*1024) << " KB\n";

    // 计算区块中的平均复合键数
    size_t key_count = 0;
    for(Block& blk : blockchain) {
        std::set<std::string> keys;
        for(transaction& tx : blk.transactions) {
            keys.insert(tx.u+tx.type);
        }
        key_count += keys.size();
    }
    std::cout << "avg compound keys per block: " << (double)key_count / (double)blockchain.size() << std::endl;
}


void experiment::test_query(std::string filename, int txs_in_one_block, std::vector<int> tw_size, 
                    std::vector<int> K_list, std::string u_q, std::string type_q){
    std::vector<transaction> transactions = get_txs(filename);
    std::vector<Block> blockchain = Block::construct_chain(transactions, txs_in_one_block);
    std::chrono::_V2::system_clock::time_point start, end;
    std::chrono::duration<double> duration;

    // 对不同组合的参数进行测试
    for(int tw: tw_size){
        for(int K: K_list) {
            // Search
            // 重复实验20次
            Response response;
            int lb = 0;
            int ub = tw-1;

            start = std::chrono::high_resolution_clock::now();
            for(int x=0; x<20; x++){
                response = Query::Search(u_q, type_q, K, lb, ub, blockchain);
            }
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            // 查询时间
            std::cout << "Search Duration: " << duration.count()/20 << " seconds" << std::endl;

            // VO Size, KB            
            double vo_size = double(test_VO_size(response)) / double(1024);
            std::cout << "VO Size: " << vo_size << " KB" << std::endl;

            // Verify
            // 重复实验20次
            start = std::chrono::high_resolution_clock::now();
            for(int x=0;x<20;x++){            
                Query::Verify(u_q, type_q, response, K, lb, ub, blockchain);
            }
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            // 验证时间
            std::cout << "Verify Duration: " << duration.count()/20 << " seconds" << std::endl;
        }
    }

}




long long experiment::test_VO_size(Response& response){
    // 每个区块的查询结果R
    std::map<int, std::vector<ListNode>>& R = response.R;
    // 每个区块中com_key_q的存在/不存在证明
    std::map<int, SMTProof>& VO = response.VO;

    // 总的存储空间
    long long total_size = 0;

    // 计算VO SIZE
    for(std::pair<int, SMTProof> pf: VO){
        std::vector<SMTNode>& subtree = pf.second.subtree;
        
        // 对subtree中所有SMT结点，计算存储空间。
        // 若是叶结点，包含com_key, h1 ---> 
        // 若是非叶结点，包含lchild/ rchild, lhash/ rhash --->
        for(SMTNode node: subtree){
            if(node.isLeaf){
                total_size = total_size + 32 + node.compound_key.first.length() + node.compound_key.second.length();
            }
            else{
                if(node.lchild == -1){
                    total_size += 32;
                }
                else{
                    total_size += 4;
                }
                if(node.rchild == -1){
                    total_size += 32;
                }
                else{
                    total_size += 4;
                }
            }
        }
    }

    // 计算R size
    for(std::pair<int, std::vector<ListNode>> pf: R){
        for(ListNode node: pf.second){
            total_size = total_size + node.v.length() + 4 + 32;
        }
    }

    return total_size;
}



size_t experiment::test_ADS_size(Block blk){
    std::vector<struct SMTNode>& tree = blk.smt.tree;
    std::map<std::pair<std::string, std::string>, std::vector<struct ListNode>>& Lists = blk.Lists;
    size_t total_size = 0;

    // 计算SMT的大小
    for(SMTNode node: tree){
        if(node.isLeaf){
            total_size = total_size + node.compound_key.first.length() + node.compound_key.second.length() + 32 + 32;
        }
        else{
            total_size += 32;
        }
    }


    // 计算Lists的大小
    for(auto pair: Lists){
        std::vector<struct ListNode>& vec = pair.second;
        for(auto node: vec){
            total_size = total_size + node.v.length() + 4 + 32;
        }
    }
    // std::cout << total_size << std::endl;
    return total_size;
}