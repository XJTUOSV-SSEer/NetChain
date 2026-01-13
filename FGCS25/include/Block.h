#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include "Structs.h"
#include "BloomFilter.h"


class Block{
private:

public:
    // 区块高度
    int blk_height;
    // 哈希指针
    std::string preBkHash;

    // skip index根哈希，计算方式为H(bf||skip_index[0]||skip_index[1]||...||skip_index[e_-1])
    std::string h_skip;

    // 块内的交易
    std::vector<transaction> transactions;

    // 此区块的布隆过滤器
    BloomFilter bf;
    // 此区块的skip index，包含e_个BF
    size_t e_;
    std::vector<BloomFilter> skip_index;




    /*
        构造函数。出块，构造MHT索引
        param:
            transactions - 要打包的交易
            m_ - BF的参数m_
            k_ - BF的参数k_
            e_ - skip index参数e_
            blockchain - 当前的区块链
    */
    Block(std::vector<transaction>& transactions, size_t m_, size_t k_, 
            size_t e_, std::vector<Block>& blockchain);


    /*
        给定交易数据集，构造区块链
        param:
            transactions - 数据集
            max_transactions - 一个区块中交易的最大数量
        return:
            模拟的区块链
    */
    static std::vector<Block> construct_chain(std::vector<transaction>& transactions, 
                                        int max_transactions, size_t m_, size_t k_, size_t e_);
};




#endif