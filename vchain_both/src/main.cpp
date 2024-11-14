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
    // experiment::adjust_dataset("../../dataset/Wiki-Vote.txt", 8, 1000, "../../dataset/dataset3.txt");
    // experiment::show_dataset("../../dataset/Wiki-Vote.txt", 0);

    // experiment::show_dataset("../../dataset/dataset1.txt", 0);

    // // 数据集 enron email，测试出块时间
    // std::string filename = "../../dataset/dataset1.txt";
    // experiment exp(filename);
    // exp.test_mining(150);

    // 数据集 GPLUS，测试出块时间
    std::string filename = "../../dataset/dataset2.txt";
    experiment exp(filename);
    exp.test_mining(500);

    // // 数据集 wiki，测试出块时间
    // std::string filename = "../../dataset/dataset3.txt";
    // experiment exp(filename);
    // exp.test_mining(100);


    // 从数据集enron email，测试Query时间
    // std::string filename = "../../dataset/dataset1.txt";
    // experiment exp(filename);
    // std::vector<int> tw_size = {200, 400, 600, 800, 1000};
    // std::vector<int> K_list = {20};
    // exp.test_query(150, tw_size, K_list, "1028");

    // std::string filename = "../../dataset/dataset1.txt";
    // experiment exp(filename);
    // std::vector<int> tw_size = {1000};
    // std::vector<int> K_list = {4, 8, 12, 16, 20};
    // exp.test_query(150, tw_size, K_list, "1028");



    // // 从数据集Gplus，测试Query时间
    // std::string filename = "../../dataset/dataset2.txt";
    // experiment exp(filename);
    // std::vector<int> tw_size = {5000, 10000, 15000, 20000, 25000};
    // std::vector<int> K_list = {20};
    // exp.test_query(500, tw_size, K_list, "116807883656585676940");


    // std::string filename = "../../dataset/dataset2.txt";
    // experiment exp(filename);
    // std::vector<int> tw_size = {25000};
    // std::vector<int> K_list = {4, 8, 12, 16, 20};
    // exp.test_query(500, tw_size, K_list, "116807883656585676940");


    // 从数据集wiki，测试Query时间
    // std::string filename = "../../dataset/dataset3.txt";
    // experiment exp(filename);
    // std::vector<int> tw_size = {200, 400, 600, 800, 1000};
    // std::vector<int> K_list = {20};
    // exp.test_query(100, tw_size, K_list, "1133");

    // std::string filename = "../../dataset/dataset3.txt";
    // experiment exp(filename);
    // std::vector<int> tw_size = {1000};
    // std::vector<int> K_list = {4, 8, 12, 16, 20};
    // exp.test_query(100, tw_size, K_list, "1133");
}