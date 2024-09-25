#include "../include/Block.h"
#include <vector>
#include <map>
#include <string>


Block::Block(std::vector<transaction>& transactions, std::string preBkHash, 
            std::map<std::pair<std::string, std::string>, std::string>& prime_map, MultisetAccumulator& msa){
    this->transactions = transactions;
    this->preBkHash = preBkHash;
    this->mht = MHT(transactions, prime_map, msa);
    this->h_mht = this->mht.tree[this->mht.root_id].digest;
}



std::vector<Block> Block::construct_chain(std::vector<transaction>& transactions, int max_transactions){
    MultisetAccumulator msa;
    std::map<std::pair<std::string, std::string>, std::string> prime_map;
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
        Block blk(batch, std::string(32, '\0'), prime_map, msa);
        blockchain.push_back(blk);
    }

    return blockchain;
}