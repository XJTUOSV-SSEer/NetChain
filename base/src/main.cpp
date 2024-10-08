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

    // // 测试出块时间，使用GPLUS数据集
    // std::string filename = "../../dataset/dataset2.txt";
    // experiment exp(filename);
    // exp.test_mining(500);

    // // 数据集 wiki，测试出块时间
    // std::string filename = "../../dataset/dataset3.txt";
    // experiment exp(filename);
    // exp.test_mining(100);


    // 从数据集enron email，测试Query时间
    std::string filename = "../../dataset/dataset1.txt";
    experiment exp(filename);
    std::vector<int> tw_size = {200, 400, 600, 800, 1000};
    std::vector<int> K_list = {20};
    exp.test_query(150, tw_size, K_list, "1028");

    // std::string filename = "../../dataset/dataset1.txt";
    // experiment exp(filename);
    // std::vector<int> tw_size = {1000};
    // std::vector<int> K_list = {4, 8, 12, 16, 20};
    // exp.test_query(150, tw_size, K_list, "1028");
}