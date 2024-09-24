#include "../include/Block.h"
#include <vector>


Block::Block(std::vector<transaction>& transactions, std::string preBkHash, 
            std::map<std::pair<std::string, std::string>, std::string>& prime_map, MultisetAccumulator& msa){
    this->transactions = transactions;
    this->preBkHash = preBkHash;
    this->mht = MHT(transactions, prime_map, msa);
    this->h_mht = this->mht.tree[this->mht.root_id].digest;
}