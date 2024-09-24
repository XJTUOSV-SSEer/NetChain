#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include "Structs.h"
#include "MHT.h"

class Block{
private:

public:
    // 哈希指针
    std::string preBkHash;

    // MHT根哈希
    std::string h_mht;

    // 块内的交易
    std::vector<transaction> transactions;

    // MHT
    MHT mht;


    /*
        构造函数。出块，构造MHT索引
        param:
            transactions - 要打包的交易
            preBkHash - 哈希指针，前一个区块块头的哈希
            prime_map - 储存混合键对应的素数
            msa - 
    */
    Block(std::vector<transaction>& transactions, std::string preBkHash, 
            std::map<std::pair<std::string, std::string>, std::string>& prime_map, MultisetAccumulator& msa);

};




#endif