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
};



#endif