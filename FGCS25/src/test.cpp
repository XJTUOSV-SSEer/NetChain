#include <cassert>
#include <iostream>
#include "../include/Structs.h"
#include "../include/Query.h"
#include "../include/Block.h"
#include <fstream>
#include <sstream>
#include <random>




/*-------------------------------------- search简单测试---------------------------*/
// int main(void){
//     std::cout<<"-----------------------test-----------------------------"<<std::endl;

//     /*-------------------------- 数据集 -------------------------------------*/
//     std::vector<transaction> transactions;

//     transactions.push_back(transaction("a", "c", "friend", 18));
//     transactions.push_back(transaction("c", "d", "friend", 20));
//     transactions.push_back(transaction("b", "e", "family", 40));
//     transactions.push_back(transaction("a", "e", "colleague", 3));
//     transactions.push_back(transaction("e", "b", "colleague", 10));
//     transactions.push_back(transaction("a", "d", "friend", 10));
//     transactions.push_back(transaction("c", "e", "friend", 9));
//     transactions.push_back(transaction("a", "b", "family", 30));
//     transactions.push_back(transaction("a", "e", "family", 20));    
//     transactions.push_back(transaction("b", "d", "colleague", 18));
//     transactions.push_back(transaction("a", "d", "colleague", 8));
//     transactions.push_back(transaction("d", "a", "colleague", 9));
//     transactions.push_back(transaction("d", "e", "colleague", 7));
//     transactions.push_back(transaction("a", "b", "friend", 20));

//     /*-------------------------- 出块 ----------------------------------------*/
//     size_t max_transactions = 3;
//     std::vector<Block> blockchain = Block::construct_chain(transactions, max_transactions, 12*max_transactions, 3, 4);

//     /*--------------------------- 查询 ----------------------------------------*/
//     std::string u_q = "a";
//     std::string type_q = "friend";
//     int lb = 0;
//     int ub = 4;

//     std::vector<Block> VO = Query::Search(u_q, type_q, lb, ub, blockchain);
//     std::vector<transaction> results = Query::Verify(u_q, type_q, lb, ub, blockchain, VO);

//     for(transaction& tx : results) {
//         std::cout << tx.u << " " << tx.v << " " << tx.type << " " << tx.w << std::endl;
//     }
// }



/*-------------------------------------- search多用例测试---------------------------*/
void gen_data(){
    std::string filename = "../test_data.txt";
    std::ofstream f(filename);

    std::vector<std::string> types = {"friend", "family", "colleague"};
    for(size_t i = 0; i < 10000; i++){
        std::string u = std::to_string((rand() % 100)+1);
        std::string v = std::to_string((rand() % 100)+1);
        std::string type = types[rand() % types.size()];
        int w = rand() % 5000;

        // 交易参数写入文件
        f << u << " " << v << " " << type << " " << w << "\n";
    }
}

int main(void) {
    // 生成数据
    // gen_data();

    std::cout<<"-----------------------test-----------------------------"<<std::endl;

        // 读取数据，构造交易
    std::vector<transaction> transactions;
    std::string filename = "../test_data.txt";
    std::ifstream f(filename);
    std::string line;
    while (std::getline(f, line)) {
        std::stringstream ss(line);
        std::string u, v, type;
        int w;
        ss >> u >> v >> type >> w;
        transactions.push_back(transaction(u, v, type, w));
    }


    /*-------------------------- 测试不同区块大小下的情况 -----------------------------------*/
    for(size_t max_transactions = 1; max_transactions <= 1000; max_transactions++){
        std::vector<Block> blockchain = Block::construct_chain(transactions, max_transactions, 12*max_transactions, 3, 7);
        std::vector<std::string> types = {"friend", "family", "colleague"};

        // 查询，遍历所有可能的<u,type>        
        for(size_t i = 1; i<=100; i++){
            for(std::string type : types){
                std::string u_q = std::to_string(i);
                std::string type_q = type;
                std::cout << "max_txs:" << max_transactions << " queried: " << u_q << " " << type_q <<std::endl;
                int lb = 0;
                int ub = blockchain.size()-1;
                std::vector<Block> VO = Query::Search(u_q, type_q, lb, ub, blockchain);
                std::vector<transaction> results = Query::Verify(u_q, type_q, lb, ub, blockchain, VO);

                // 与正确结果对比
                std::vector<transaction> results_;      // 储存正确结果
                for(int j = ub; j >=0; j--){
                    Block& blk = blockchain[j];
                    for(size_t k = 0; k < blk.transactions.size(); k++) {
                        transaction& tx = blk.transactions[k];
                        if(tx.u == u_q && tx.type == type_q) {
                            results_.push_back(tx);
                        }
                    }
                }
                assert(results.size() == results_.size());
                for(size_t j = 0; j < results.size(); j++){
                    transaction& tx = results[j];
                    transaction& tx_ = results_[j];
                    assert(tx.u == tx_.u && tx.v == tx_.v && tx.type==tx_.type && tx.w==tx_.w);
                }
            }
        }
    }
}