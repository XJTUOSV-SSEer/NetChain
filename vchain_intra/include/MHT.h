#ifndef MHT_H
#define MHT_H

#include <vector>
#include "Structs.h"

class MHT{
private:
    /*
        计算两个集合之间的相似度

        param:
            a, b - 混合键组成的集合
        return:
            雅可比相似度，取值为[0, 1]
    */
    static double get_similarity(std::set<std::pair<std::string, std::string>>& a, 
                                std::set<std::pair<std::string, std::string>>& b);
public:
    // 储存MHT中所有结点
    std::vector<MHTNode> tree;

    // 根结点在tree数组中的下标
    int root_id;


    /*
        构造函数
        param:
            txs - 若干交易
            prime_map - 储存每个混合键对应的素数。用于打表快速计算素数
            msa - 
    */
    MHT(std::vector<transaction> txs, std::map<std::pair<std::string, std::string>, std::string>& prime_map, 
            MultisetAccumulator& msa);
    
    MHT();


    /*
        给定混合键，搜索得到MHT中所有匹配的叶结点。
        具体算法使用递归（没有使用文中介绍的层序遍历，因为难以构造proof subtree）
        (1) 对当前处理的结点，若包含混合键且是叶结点，将叶结点复制一份加入proof.subtree；
        (2) 若包含混合键且是非叶结点，递归地调用子结点，然后将该结点加入proof
        (3) 若不包含混合键，做NonMembership proof，并将该结点加入proof

        param:
            mht_proof - MHT proof的引用
            current_id - 当前处理的结点在MHT.tree数组中的下标
            com_key_q - 
            msa - 
            p - com_key_q对应的素数
        return:
            若当前结点被加入了proof.subtree，返回其在proof.subtree中的下标；否则返回-1
    */
    int search(MHTProof& mht_proof, int current_id, std::pair<std::string, std::string> com_key_q,
                MultisetAccumulator& msa, std::string p);
};



#endif