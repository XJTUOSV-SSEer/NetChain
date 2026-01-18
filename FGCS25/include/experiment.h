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
    // static int test_VO_size(Response& response);


    /*
        计算ADS size，返回字节数
    */
    // static int test_ADS_size(Block blk);
    
public:
    std::vector<transaction> transactions;

    /*
        读入数据集，初始化
        param:
            filename - 数据集的文件名
            num - 数据集中记录的数量
    */
    experiment(std::string filename);

    static void test_query(std::string filename, int txs_in_one_block, std::vector<int> tw_size,
                            std::string u_q, std::string type_q);


};



#endif