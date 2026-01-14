#include <vector>
#include <map>
#include <string>
#include "../include/Block.h"

Block::Block(std::vector<transaction>& transactions, size_t m_, size_t k_, 
            size_t e_, std::vector<Block>& blockchain){
    this->blk_height = blockchain.size();
    this->transactions = transactions;
    this->preBkHash = std::string(32, '\0');
    this->e_ = e_;

    // 初始化内部的BF和skip index
    this->bf = BloomFilter(m_, k_);
    for(size_t j=0; j<e_; j++){
        this->skip_index.push_back(BloomFilter());
    }

    // 将区块内的关键字插入BF。这里插入的是u||type形式的关键字
    for(transaction tx : transactions){
        std::string kw = tx.u + tx.type;
        this->bf.add(kw);
    }

    // 构造skip index。假设当前区块的id为i，则skip index中第j个BF可通过第i-2^j个块中第j-1个BF和第i-2^(j-1)个块中第j-1个BF做按位OR操作得到。
    // 边界条件：若某个BF对应的区块范围小于创世区块，则将BF置为全0
    int i = blockchain.size();
    // 处理j=0
    if(i > 0){
        this->skip_index[0] = blockchain[i - 1].bf;
    }
    else{
        this->skip_index[0] = BloomFilter(m_, k_);          // 置为空
    }
    // 处理j=[1,...,e_-1]
    for(int j = 1; j < e_; j++){
        int c1 = i - (1 << (j-1));      // 第i-2^(j-1)个块
        int c2 = i - (1 << j);          // 第i-2^j个块
        // 若块号i-2^(j-1) < 0，则使用空BF代为计算
        BloomFilter bf_c1 = (c1 >= 0) ? blockchain[c1].skip_index[j-1] : BloomFilter(m_, k_);
        BloomFilter bf_c2 = (c2 >= 0) ? blockchain[c2].skip_index[j-1] : BloomFilter(m_, k_);

        // 两个子BF进行merge得到skip index中第j个BF
        this->skip_index[j] = BloomFilter(m_, k_);
        for(size_t pos=0; pos < m_; pos++){
            this->skip_index[j].bits_[pos] = bf_c1.bits_[pos] || bf_c2.bits_[pos];
        }
    }

    // 级联bf以及skip index中所有布隆过滤器，并计算哈希
    std::string msg = this->bf.bin_form();
    for(size_t j=0; j<e_; j++){
        msg += this->skip_index[j].bin_form();
    }
    this->h_skip = Crypto_Primitives::SHA256_digest(msg);
}






std::vector<Block> Block::construct_chain(std::vector<transaction>& transactions, int max_transactions, 
                                        size_t m_, size_t k_, size_t e_){
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
        Block blk(batch, m_, k_, e_, blockchain);
        blockchain.push_back(blk);
    }

    return blockchain;
}