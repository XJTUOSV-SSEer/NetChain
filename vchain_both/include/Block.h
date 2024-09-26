#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include "Structs.h"
#include "MHT.h"

class Block{
private:
    /*
        为一个区块构造跳表。需要注意，此时blk_id对应的区块还没有加入blockchain
        param:
            blk_id - 当前区块加入后的区块号
            blockchain - 区块链的引用
            level - 跳表的级数
            latest_set - blk_id对应的区块的混合键集合
            prime_map - 储存混合键对应的素数，用于打表
    */
    void get_SkipList(int blk_id, std::vector<Block>& blockchain, int level, 
            std::set<std::pair<std::string, std::string>>& latest_set,
            std::map<std::pair<std::string, std::string>, std::string>& prime_map);

public:
    // 哈希指针
    std::string preBkHash;

    // MHT根哈希
    std::string h_mht;

    // 跳表的哈希
    std::string h_sl;

    // 块内的交易
    std::vector<transaction> transactions;

    // MHT
    MHT mht;

    // 跳表
    SkipList skiplist;


    /*
        构造函数。出块，构造MHT索引
        param:
            transactions - 要打包的交易
            preBkHash - 哈希指针，前一个区块块头的哈希
            prime_map - 储存混合键对应的素数
            msa - 
            blockchain - 区块链的引用。注意，此时当前块尚未加入区块链
    */
    Block(std::vector<transaction>& transactions, std::string preBkHash, 
            std::map<std::pair<std::string, std::string>, std::string>& prime_map, MultisetAccumulator& msa,
            std::vector<Block>& blockchain);


    /*
        给定交易数据集，构造区块链
        param:
            transactions - 数据集
            max_transactions - 一个区块中交易的最大数量
        return:
            模拟的区块链
    */
    static std::vector<Block> construct_chain(std::vector<transaction>& transactions, int max_transactions);
};




#endif