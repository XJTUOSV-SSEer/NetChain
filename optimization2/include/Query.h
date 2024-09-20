// 查询的两个接口：Search, Verify
#ifndef QUERY_H
#define QUERY_H

#include <string>
#include <vector>
#include "Block.h"
#include "Structs.h"

class Query{
private:
    static bool compare_by_w(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b);
public:
    /*
        模拟全节点对区块链进行搜索。
        使用两次扫描的策略：第一次扫描找到全局的top-K结果；第二次扫描再判断每个匹配的结点为全局结果贡献了多少元素。
        param:
            u_q - 查询的点
            type_q - 查询的边的类型
            K - top-K的参数K
            lb - 查询区间的下界（区块号）
            ub - 查询区间的上界
            chain - 区块链的引用
        return:

    */
    static Response Search(std::string u_q, std::string type_q, int K, int lb, int ub, std::vector<Block>& chain);


    /*
        模拟轻节点对查询结果验证，包括正确性与完整性两部分验证
        param:
            u_q
            type_q
            response - 全节点返回的查询结果
            K -
            lb,ub - 查询的区块号的区间：[lb,ub]
            chain - 区块链的引用
        return:
            全局top-K结果，根据权重w排序。数组中的元素为二元组<v,w>
    */
    static std::vector<std::pair<std::string, int>> Verify(std::string u_q, std::string type_q, 
                            Response& response, int K, int lb, int ub, std::vector<Block>& chain);
};



#endif