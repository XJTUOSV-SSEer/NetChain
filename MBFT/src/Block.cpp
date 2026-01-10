#include "../include/Block.h"
#include <vector>
#include <map>
#include <string>


Block::Block(std::vector<transaction>& transactions, size_t original_m_, size_t original_k_, size_t original_len_h_){
    this->transactions = transactions;
    this->preBkHash = std::string(32, '\0');
    this->mbft = MBFT(transactions, original_m_, original_k_, original_len_h_);
    this->h_mbft = this->mbft.tree[mbft.tree.size()-1].digest;
}



std::vector<Block> Block::construct_chain(std::vector<transaction>& transactions, int max_transactions, 
                                        size_t original_m_, size_t original_k_, size_t original_len_h_){
    std::vector<Block> blockchain;
    

    // 当前批次的交易
    std::vector<transaction> batch;
    for(int i=0; i<transactions.size(); i += max_transactions){
        // 确定本区块的交易个数num_tx
        int num_tx;
        if(i+max_transactions > transactions.size()){
            num_tx = transactions.size() - i;
            std::vector<transaction> tmp(transactions.begin()+i, transactions.end());
            batch = tmp;
        }
        else{
            num_tx = max_transactions;
            std::vector<transaction> tmp(transactions.begin()+i, transactions.begin()+i+num_tx);
            batch = tmp;
        }

        // 出块
        Block blk(batch, original_m_, original_k_, original_len_h_);
        blockchain.push_back(blk);
    }

    return blockchain;
}