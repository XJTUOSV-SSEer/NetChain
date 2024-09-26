#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <vector>
#include "Structs.h"
#include <string>

class experiment{
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
};



#endif