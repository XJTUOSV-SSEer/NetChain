#include "../include/Block.h"
#include "../include/Crypto_Primitives.h"
#include <vector>
#include <map>
#include <cmath>
#include <string>



Block::Block(std::vector<transaction>& transactions, std::string preBkHash, 
            std::map<std::pair<std::string, std::string>, std::string>& prime_map, MultisetAccumulator& msa,
            std::vector<Block>& blockchain){
    this->transactions = transactions;
    this->preBkHash = preBkHash;
    this->mht = MHT(transactions, prime_map, msa);
    this->h_mht = this->mht.tree[this->mht.root_id].digest;

    // 构造跳表
    get_SkipList(blockchain.size(), blockchain, 5, mht.tree[mht.root_id].com_key_set, prime_map);
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
        Block blk(batch, std::string(32, '\0'), prime_map, msa, blockchain);
        blockchain.push_back(blk);
    }

    return blockchain;
}



void Block::get_SkipList(int blk_id, std::vector<Block>& blockchain, int level,
                    std::set<std::pair<std::string, std::string>>& latest_set, 
                    std::map<std::pair<std::string, std::string>, std::string>& prime_map){

    MultisetAccumulator msa;

    // 对不同的skip，建立SkipListNode
    // skip升序
    for(int i=1; i<=level; i++){
        SkipListNode node;

        // 当前的skip = 2^i
        int skip = std::pow(2, i);

        // 判断当前区块及之前的区块数是否小于skip。若小于，则不继续构造该skip的SkipListNode
        if(blk_id+1 < skip){            
            node.is_empty = true;
            node.skip = skip;
        }

        // 构造该skip的SkipListNode
        else{
            node.is_empty = false;
            node.skip = skip;

            // 将skip内所有区块的混合键求并集
            std::set<std::pair<std::string, std::string>> set_union = latest_set;        // 储存并集
            for(int j = blk_id+1-skip; j<blk_id; j++){
                // 取出该区块中的混合键集合
                MHT& mht = blockchain[j].mht;
                std::set<std::pair<std::string, std::string>>& key_set = mht.tree[mht.root_id].com_key_set;

                // 并集
                std::set<std::pair<std::string, std::string>> tmp_set;
                std::set_union(set_union.begin(), set_union.end(), key_set.begin(), key_set.end(), 
                        std::inserter(tmp_set, tmp_set.begin()));
                set_union = tmp_set;
            }

            // 计算set_union对应的acc和product
            std::set<std::string> x_set;            // 储存集合中混合键对应的素数
            BigInteger product_bi(1);               // 素数乘积
            for(std::pair<std::string, std::string> com_key: set_union){
                // 计算混合键对应的素数
                BigInteger p_bi;

                if(prime_map.find(com_key) != prime_map.end()){
                    p_bi.reset(prime_map[com_key]);
                }
                else{
                    BigInteger::generate_prime(p_bi, com_key.first + com_key.second);
                    // 更新表
                    prime_map[com_key] = p_bi.to_string();
                }
                BigInteger::mul(product_bi, product_bi, p_bi);

                // 素数集合
                x_set.insert(p_bi.to_string());
            }

            // 生成acc
            node.acc = msa.get_acc_prime(x_set);
            node.product = product_bi.to_string();
            node.com_key_set = set_union;
        }

        // node的digest
        node.digest = Crypto_Primitives::SHA256_digest(node.acc);

        // 将ListNode加入跳表
        this->skiplist.vec.push_back(node);
    }

    // 整个跳表的digest
    std::string msg = "";
    for(SkipListNode& node: this->skiplist.vec){
        msg += node.digest;
    }
    this->skiplist.digest = Crypto_Primitives::SHA256_digest(msg);

    this->h_sl = this->skiplist.digest;
}