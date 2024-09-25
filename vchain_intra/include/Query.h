#ifndef QUERY_H
#define QUERY_H

#include <vector>
#include "Block.h"


class Query{
private:
    /*
        验证以当前结点为root的子树，并返回当前结点的digest
        param:
            subtree - MHT证明子树
            proof - 不匹配的结点的non-membership proof
            current_id - 当前结点在subtree中的下标
            res - 储存匹配的结果<v,w>
            msa -
            p - 混合键对应的素数
    */
    static std::string traverse(std::vector<MHTNode>& subtree, std::map<int, Nonmembership_Proof>& proof, 
                                int current_id, std::vector<std::pair<std::string, int>>& res, 
                                MultisetAccumulator& msa, std::string p);

    
    static bool compare_by_w(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b);
public:
    /*
        模拟SP进行搜索。由于没有MHT的叶结点无序，因此只能将区块中所有匹配com_key_q的叶结点都返回
        param:
            blockchain - 区块链
            u_q, type_q - 要查询的键
            K - 参数
            lb, ub - time window
        return:
            
    */
    static Response search(std::vector<Block>& blockchain, std::string u_q, std::string type_q, int K, 
                            int lb, int ub);

    /*
        client对SP返回的response验证，并得到全局top-K结果
    */
    static std::vector<std::pair<std::string, int>> verify(std::vector<Block>& blockchain, 
                std::string u_q, std::string type_q, int K, int lb, int ub, Response& response);
};


#endif