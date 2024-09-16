// 查询的两个接口：Search, Verify
#ifndef QUERY_H
#define QUERY_H

#include <string>
#include <vector>
#include "Block.h"
#include "Structs.h"

class Query{
public:
    /*
        模拟全节点对区块链进行搜索
        param:
            u_q - 查询的点
            type_q - 查询的边的类型
            K - top-K的参数K
            lb - 查询区间的下界（区块号）
            ub - 查询区间的上界
            chain - 区块链的引用
        return:

    */
    static void Search(std::string u_q, std::string type_q, int K, int lb, int ub, std::vector<Block>& chain);

    static void Verify();
};



#endif