#include <iostream>
#include <vector>

#include "../include/Query.h"
#include "../include/Block.h"
#include "../include/Structs.h"
#include "../include/experiment.h"
#include <chrono>

/*
    测实验数据
*/
int main(void){
    // 数据集 enron email，测试出块时间
    std::string filename = "../../dataset/dataset1.txt";
    experiment exp(filename);
    exp.test_mining(150);

    // // 数据集 GPLUS，测试出块时间
    // std::string filename = "../../dataset/dataset2.txt";
    // experiment exp(filename);
    // exp.test_mining(500);
    
}