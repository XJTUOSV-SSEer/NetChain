// https://snap.stanford.edu/data/index.html
// https://snap.stanford.edu/data/soc-sign-bitcoin-otc.html


// gplus dataset: 72271 nodes, 13673453 edges
// query node_id: 116807883656585676940, corresponding edges: 4879



#include <iostream>
#include <vector>
#include "../include/SMT.h"
#include "../include/Block.h"
#include "../include/SMT.h"
#include "../include/Structs.h"
#include "../include/Query.h"
#include "../include/experiment.h"
#include <chrono>



int main(void){
    // // 测试出块时间，使用Enron Email数据集
    // std::string filename = "../../dataset/dataset1.txt";
    // experiment exp(filename);
    // exp.test_mining(150);

    // 测试出块时间，使用GPLUS数据集
    std::string filename = "../../dataset/dataset2.txt";
    experiment exp(filename);
    exp.test_mining(500);
}