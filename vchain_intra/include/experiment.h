#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <vector>
#include "Structs.h"
#include "Block.h"
#include <string>

class experiment{
private:
    /*
        给定response，测试VO size，返回字节数
    */
    static long long test_VO_size(Response& response);


    /*
        计算ADS size，返回字节数
    */
    static size_t test_ADS_size(Block blk);

    /*
        读取数据集，得到所有交易
    */
    static std::vector<transaction> get_txs(std::string filename);
    
public:
    /*
        txs_in_one_block - 一个块中的交易数
    */
    static void test_mining(std::string filename, int txs_in_one_block);

    /*
        给定time window size和参数K，测试search和verify的时间。
        具体地，首先构建区块链，然后对所有time window size和K的组合，测试query performance
        param:
            txs_in_one_block - 
            tw_size - 一个vector容器，储存不同的time window size
            K_list - 一个vector容器，储存不同的参数K
            u_q - 查询的键
    */
    static void test_query(std::string filename, int txs_in_one_block, std::vector<int> tw_size, 
                    std::vector<int> K_list, std::string u_q, std::string type_q);
};



#endif