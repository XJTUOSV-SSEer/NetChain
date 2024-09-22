#include "../include/Block.h"
#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include "../include/Crypto_Primitives.h"
#include "../include/MPT.h"
#include <iostream>


// 用于对list根据权重进行降序排序
bool compare_by_w(const ListNode& a, const ListNode& b){
    return a.w > b.w;
}


Block::Block(int blk_id, std::string pre_hash, std::vector<struct transaction>& transactions, MPT& mpt){
    this->blk_id = blk_id;
    this->pre_hash = pre_hash;
    this->transactions = transactions;

    // 首先对每个混合键，找到对应的List
    for(int i=0; i<transactions.size(); i++){
        struct transaction obj = transactions[i];
        // 提取混合键
        std::pair<std::string, std::string> k = std::make_pair(obj.u, obj.type);
        // 判断该混合键是否已在Lists map中。
        // 若是，更新对应的sorted List；否则，在map中新建条目
        if(Lists.find(k) == Lists.end()){
            std::vector<struct ListNode> tmp;
            Lists[k] = tmp;
        }
        ListNode ln(obj.v, obj.w);
        Lists[k].push_back(ln);
    }

    // 储存所有混合键对应的SMT叶结点
    std::vector<struct SMTNode> leaves;

    // 对map中所有混合键对应的list，进行排序，并计算每个node的哈希指针
    for(std::map<std::pair<std::string, std::string>, std::vector<struct ListNode>>::iterator it = Lists.begin();
        it!= Lists.end(); it++){
        // 当前混合键
        std::pair<std::string, std::string> k = it->first;

        // 当前混合键对应的list
        std::vector<ListNode>& list = it->second;

        // 根据权重w对list排序
        std::sort(list.begin(), list.end(), compare_by_w);

        // 计算list中每个结点的哈希指针
        for(int i=list.size()-1; i>=0; i--){
            // 若为最后一个元素，不处理
            if(i==list.size()-1){
                continue;
            }

            // 若不是最后一个元素
            else{
                // 取出后一个元素的v, w, ptr
                std::string v = list[i+1].v;
                int w = list[i+1].w;
                std::string p = list[i+1].ptr;
                // 计算哈希指针
                list[i].ptr = Crypto_Primitives::SHA256_digest(v+std::to_string(w)+p);
            }
        }

        // 构造该混合键对应的SMT leaf node
        // list头结点的哈希值
        std::string v = list[0].v;
        int w = list[0].w;
        std::string p = list[0].ptr;
        std::string h1 = Crypto_Primitives::SHA256_digest(v+std::to_string(w)+p);
        int id_pre = mpt.search(k);
        // 构造叶结点并加入leaves集合
        SMTNode leaf(k.first, k.second, list.size(), h1, id_pre);
        leaves.push_back(leaf);

        // 更新MPT树，将混合键对应的latest_blk_id更新为当前块的id
        mpt.update(k, blk_id);
    }

    // 构造SMT，并设置块头中的H_SMT
    smt = SMT(leaves);
    h_smt = smt.tree[smt.root_id].digest;

    // 将MPT的根哈希存入块头
    this->h_mpt = mpt.tree[mpt.root_id].get_hash();
}






std::vector<Block> Block::construct_chain(std::vector<transaction>& transactions, int max_transactions){
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
        Block blk(int(i/max_transactions), std::string(32, '\0'), batch);
        blockchain.push_back(blk);
    }

    return blockchain;
}