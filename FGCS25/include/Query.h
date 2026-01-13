#ifndef QUERY_H
#define QUERY_H

#include <vector>
#include "Block.h"
#include <string>

class Query{
    /*
        查询块号在[lb, ub]内的所有区块中，包含u_q||type_q的所有交易。
        所有被访问到的区块（搜索中的锚点）都被加入VO并返回
    */
    static std::vector<Block> Search(std::string u_q, std::string type_q, size_t lb, size_t ub, std::vector<Block>& chain);
};


#endif