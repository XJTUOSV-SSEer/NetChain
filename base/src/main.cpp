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
    // // 测试出块时间，使用GPLUS数据集
    // std::string filename = "../../dataset/dataset1.txt";
    // experiment::test_mining(filename, 500);

    // // 测试出块时间，使用paysim数据集
    // std::string filename = "../../dataset/dataset2.txt";
    // experiment::test_mining(filename, 500);

    // // 测试出块时间，使用gowalla数据集
    // std::string filename = "../../dataset/dataset3.txt";
    // experiment::test_mining(filename, 500);

    // // 测试出块时间，使用lastfm数据集
    // std::string filename = "../../dataset/dataset4.txt";
    // experiment::test_mining(filename, 500);


    // // 从数据集Gplus，测试Query时间
    // std::string filename = "../../dataset/dataset1.txt";
    // std::vector<int> tw_size = {5000, 10000, 15000, 20000, 25000};
    // std::vector<int> K_list = {20};
    // experiment::test_query(filename, 500, tw_size, K_list, "1916398003", "colleague");

    // // 以Gplus为例测试参数k的影响
    // std::string filename = "../../dataset/dataset1.txt";
    // std::vector<int> tw_size = {25000};
    // std::vector<int> K_list = {2, 4, 6, 8, 10};
    // experiment::test_query(filename, 500, tw_size, K_list, "1916398003", "colleague");


    // // 从数据集paysim，测试Query时间
    // std::string filename = "../../dataset/dataset2.txt";
    // std::vector<int> tw_size = {2000, 4000, 6000, 8000, 10000};
    // std::vector<int> K_list = {20};
    // experiment::test_query(filename, 500, tw_size, K_list, "0935995067", "PAYMENT");


    // 从数据集gowalla，测试Query时间
    std::string filename = "../../dataset/dataset3.txt";
    std::vector<int> tw_size = {40000, 80000, 120000, 160000, 200000};
    std::vector<int> K_list = {20};
    experiment::test_query(filename, 500, tw_size, K_list, "2134270587", "friend");


    // // 从数据集gowalla，测试Query时间
    // std::string filename = "../../dataset/dataset4.txt";
    // std::vector<int> tw_size = {700, 1400, 2100, 2800, 3500};
    // std::vector<int> K_list = {20};
    // experiment::test_query(filename, 500, tw_size, K_list, "0732136308", "interest");
}