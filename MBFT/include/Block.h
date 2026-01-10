#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include "Structs.h"
#include "./MBFT.h"


class Block{
private:

public:
    // 哈希指针
    std::string preBkHash;

    // MBFT根哈希
    std::string h_mbft;

    // 块内的交易
    std::vector<transaction> transactions;

    // MHT
    MBFT mbft;


    /*
        构造函数。出块，构造MHT索引
        param:
            transactions - 要打包的交易
            original_m_ - 叶结点中MBF的参数m_
            original_k_ - 叶结点中MBF的参数k_
            original_len_h_ - 叶结点中MBF的参数len_h_
    */
    Block(std::vector<transaction>& transactions, size_t original_m_, size_t original_k_, size_t original_len_h_);


    /*
        给定交易数据集，构造区块链
        param:
            transactions - 数据集
            max_transactions - 一个区块中交易的最大数量
        return:
            模拟的区块链
    */
    static std::vector<Block> construct_chain(std::vector<transaction>& transactions, 
                                        int max_transactions, size_t original_m_, size_t original_k_, size_t original_len_h_);
};




#endif