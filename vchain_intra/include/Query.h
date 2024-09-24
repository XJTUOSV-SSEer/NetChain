#ifndef QUERY_H
#define QUERY_H

#include <vector>
#include "Block.h"


class Query{
private:
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

    static void verify();
};


#endif