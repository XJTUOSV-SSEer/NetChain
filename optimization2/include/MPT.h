// class for Merkle Patricia Trie (MPT)

#ifndef MPT_H
#define MPT_H

#include <vector>
#include <string>
#include <set>
#include <map>
#include "Crypto_Primitives.h"
#include "Structs.h"


// 本质上是前缀树+MHT
class MPT{
private:

public:
    /*
        用于前缀树寻找路径。输入路径中的一个字符，返回这个字符在指针数组中对应的下标
    */
    static int get_order(std::string s);

    // 储存MPT树中的结点
    std::vector<MPTNode> tree;

    // 根结点在tree中的下标
    int root_id;

    /*
        更新某个混合键对应的latest_blk_id
        若混合键对应的路径不存在，则创建路径
        param:
            com_key - 更新的混合键
            blk_id - 更新后，该混合键最近记录所在的区块
        return:
            最新的MPT根哈希
    */
    std::string update(std::pair<std::string, std::string> com_key, int blk_id);


    /*
        查询混合键最近记录所在的区块号。若混合键尚不存在，返回-1
        param:
            com_key - 混合键
        return:
            混合键最近记录所在区块号
    */
    int search(std::pair<std::string, std::string> com_key);



    /*
        对给定的混合键，找到对应叶结点并生成merkle proof
    */
    MPTProof proveExistence(std::pair<std::string, std::string> com_key);



    /*
        验证MPT proof
        param:
            com_key - 要验证的混合键
            latest_blk - 服务器宣称的该混合键对应的latest_blk_id
            proof
            h_mpt - 块头中保存的最新MPT的根哈希
    */
    bool verifyExistence(std::pair<std::string, std::string> com_key, int latest_blk, MPTProof proof, std::string h_mpt);


    /*
        构造函数，初始化一个根结点
    */
    MPT();

};




#endif