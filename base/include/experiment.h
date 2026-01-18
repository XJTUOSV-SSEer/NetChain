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
    static int test_VO_size(Response& response);


    /*
        计算ADS size，返回字节数
    */
    static int test_ADS_size(Block blk);
    
public:
    std::vector<transaction> transactions;

    /*
        读入数据集，初始化
        param:
            filename - 数据集的文件名
            num - 数据集中记录的数量
    */
    experiment(std::string filename);



    /*
        对数据集进行调整：首先将数据集汇总为multi-map。若某个k对应多个v，将v分组。然后将得到的各个k对应的多个组进行
        shuffle打乱。从而保证k的局部连续性，出现在部分块之中。出块前，为每条记录随机生成[0,ub]内的权重

        param:
            filename
            group_size
            ub
            target_file - 目标文件
    */
    static void adjust_dataset(std::string filename, int group_size, int ub, std::string target_file);


    /*
        输入原始数据集，将其中重复的边过滤掉，保证所有边都是unique
    */
    static void filterate(std::string filename);


    /*
        输出数据集的一些统计信息，如每个k对应的v数量
    */
    static void show_dataset(std::string filename, int num);


    /*
        将paysim数据集中的账户交易信息转换为交易，并储存在target_file中
    */
    static void process_paysim_dataset(std::string filename, std::string target_file);

    /*
        对paysim数据集进行统计分析，寻找对应交易数最多的复合键
    */
    static void stat_paysim_dataset(std::string filename);


    /*
        txs_in_one_block - 一个块中的交易数
    */
    void test_mining(int txs_in_one_block);
    

    /*
        给定time window size和参数K，测试search和verify的时间。
        具体地，首先构建区块链，然后对所有time window size和K的组合，测试query performance
        param:
            txs_in_one_block - 
            tw_size - 一个vector容器，储存不同的time window size
            K_list - 一个vector容器，储存不同的参数K
            u_q - 查询的键
    */
    void test_query(int txs_in_one_block, std::vector<int> tw_size, std::vector<int> K_list, std::string u_q);
};



#endif