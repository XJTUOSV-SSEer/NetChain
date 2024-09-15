#ifndef BLOCK_H
#define BLOCK_H


#include <vector>
#include <map>
#include <string>
#include "SMT.h"

/*
    每个混合键对应sorted List中的结点
*/
struct ListNode{
    // 点v
    std::string v;
    // 权重w
    int w;
    // 哈希指针ptr
    std::string ptr;

    ListNode(std::string v, int w){
        this->v = v;
        this->w = w;
        // 哈希指针默认设置为空, 即32字节全0字符串
        this->ptr = std::string(32, '\0');
    }
};



/*
    区块中的交易对象
*/
struct transaction{
    std::string u;
    std::string v;
    std::string type;
    int w;

    transaction(std::string u, std::string v, std::string type, int w):u(u), v(v), type(type), w(w){}
};


class Block{
private:


public:
    // block header
    // 区块号
    int blk_id;
    // previous block hash
    std::string pre_hash;
    // H_SMT
    std::string h_smt;

    // block body
    // 区块中所有交易对象
    std::vector<struct transaction> transactions;

    // SMT
    SMT smt;

    // sorted lists, 储存当前区块中所有混合键对应的sorted List
    // 数据结构为一个散列表容器，键为混合键<u, type> ，值为该混合键对应的sorted List
    std::map<std::pair<std::string, std::string>, std::vector<struct ListNode>> Lists;

    /*
        构造函数，构造一个新的区块
    */
    Block(int blk_id, std::string pre_hash, std::vector<struct transaction>& transactions);    
};



#endif