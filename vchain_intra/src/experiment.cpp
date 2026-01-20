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
#include "../include/Query.h"
#include "../include/Block.h"



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
    std::cout << "Mining Duration: " << duration.count() << " seconds" << std::endl;

    // 区块中ADS的平均大小/KB
    long long total_size = 0;
    for(size_t i=0; i<blockchain.size(); i++){
        total_size = total_size + test_ADS_size(blockchain[i]);
    }
    std::cout << "ADS SIZE:" << (double)total_size/(double)(blockchain.size()*1024)<<std::endl;
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
            start = std::chrono::high_resolution_clock::now();

            int lb = 0;
            int ub = tw-1;
            Response response = Query::search(blockchain, u_q, type_q, K, lb, ub);

            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            // 查询时间
            std::cout << "Search Duration: " << duration.count() << " seconds" << std::endl;

            // VO Size, KB
            double vo_size = double(test_VO_size(response)) / double(1024);
            std::cout << "VO Size: " << vo_size << std::endl;

            // Verify
            start = std::chrono::high_resolution_clock::now();

            Query::verify(blockchain, u_q, type_q, K, lb, ub, response);

            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            // 验证时间
            std::cout << "Verify Duration: " << duration.count() << " seconds" << std::endl;
        }
    }
}




long long experiment::test_VO_size(Response& response){
    // 每个区块对应的证明子树
    std::map<int, MHTProof>& VO = response.VO;

    // 总的存储空间
    long long total_size = 0;

    // 计算VO SIZE
    for(std::pair<int, MHTProof> pf: VO){
        MHTProof& mhtproof = pf.second;
        
        // 对subtree中所有MHT结点，计算存储空间。
        // 若是叶结点且match，包含value, acc, ---> 
        // 若是非叶结点且match，包含lchild, rchild, acc ---> 40 bytes
        // 若 not match，包含acc, digest ---> 64 bytes
        for(int i = 0; i<mhtproof.subtree.size(); i++){
            // match
            if(mhtproof.proof.find(i) == mhtproof.proof.end()){
                MHTNode& node = mhtproof.subtree[i];
                // 叶结点
                if(node.isLeaf){
                    total_size = total_size + 36 + node.value.first.length();
                }
                else{
                    total_size += 40;
                }
            }

            // not match
            else{
                total_size += 64;
            }
        }


        // 对所有non-membership proof，计算存储空间，主要是两个大整数对应的字符串
        for(std::pair<int, Nonmembership_Proof> nmproof: mhtproof.proof){
            total_size = total_size + nmproof.second.a.length() + nmproof.second.d.length();
        }
    }

    return total_size;
}




size_t experiment::test_ADS_size(Block blk){
    MHT& mht = blk.mht;
    std::vector<MHTNode>& tree = mht.tree;
    size_t total_size = 0;

    for(MHTNode node : tree){
        if(node.isLeaf){
            total_size = total_size + node.value.first.length() + 4 + 32 + 32;
        }
        else{
            total_size += 64;
        }

        for(auto pair: node.com_key_set){
            total_size = total_size + pair.first.length() + pair.second.length();
        }
    }

    return total_size;
}