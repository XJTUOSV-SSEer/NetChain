#include <iostream>
#include <vector>

#include "../include/Query.h"
#include "../include/Block.h"
#include "../include/Structs.h"
#include "../include/experiment.h"
#include <chrono>




/*---------------------------- 功能测试 ---------------------------*/
// int main(void){
//     std::cout<<"-----------------------test-----------------------------"<<std::endl;

//     /*-------------------------- 数据集 -------------------------------------*/
//     std::vector<transaction> transactions;
    
//     transactions.push_back(transaction("4", "5", "colleague", 7));
//     transactions.push_back(transaction("1", "3", "friend", 18));
//     transactions.push_back(transaction("1", "2", "friend", 10));
//     transactions.push_back(transaction("3", "4", "friend", 20));
//     transactions.push_back(transaction("3", "5", "friend", 9));
//     transactions.push_back(transaction("4", "1", "colleague", 9));
//     transactions.push_back(transaction("1", "2", "family", 30));
//     transactions.push_back(transaction("1", "5", "family", 20));
//     transactions.push_back(transaction("2", "5", "family", 40));
//     transactions.push_back(transaction("1", "5", "colleague", 3));
//     transactions.push_back(transaction("5", "2", "colleague", 10));
//     transactions.push_back(transaction("2", "4", "colleague", 18));
//     transactions.push_back(transaction("1", "4", "colleague", 8));
//     transactions.push_back(transaction("1", "4", "friend", 31));
    

//     // 出块
//     std::vector<Block> blockchain = Block::construct_chain(transactions, 20);

//     // 查询
//     std::string u_q = "1";
//     std::string type_q = "friend";
//     int K=3;
//     int lb = 0;
//     int ub = blockchain.size()-1;

//     Response response = Query::search(blockchain, u_q, type_q, K, lb, ub);

//     std::vector<std::pair<std::string, int>> final_result= Query::verify(blockchain, u_q, type_q, K, lb, ub, response);

//     for(int i=0; i<final_result.size(); i++){
//         std::cout << final_result[i].first << "," << final_result[i].second <<std::endl;
//     }
// }




/*-------------------------------- 大数据集测试 --------------------------------------*/
// int main(void){
//     std::string filename = "../../dataset/dataset2.txt";
//     experiment exp(filename);


//     std::vector<transaction>& transactions = exp.transactions;

//     /*-------------------------- 出块 ----------------------------------------*/
//     auto start = std::chrono::high_resolution_clock::now();
//     std::vector<Block> blockchain = Block::construct_chain(transactions, 1000);

//     auto end = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double> duration = end - start;
//     // 输出执行时间
//     std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    

//     /*--------------------------- 查询 ----------------------------------------*/
//     std::string u_q = "116807883656585676940";
//     std::string type_q = "friend";
//     int K=100;
//     int lb = 0;
//     int ub = blockchain.size()-1;
//     // int ub = 310;

    
//     Response response = Query::search(blockchain, u_q, type_q, K, lb, ub);

//     std::vector<std::pair<std::string, int>> final_result= Query::verify(blockchain, u_q, type_q, K, lb, ub, response);
    
    
    
    
//     for(int i=0; i<final_result.size(); i++){
//         std::cout << final_result[i].first << "," << final_result[i].second <<std::endl;
//     }
// }




/*-------------------------------- 实验数据 --------------------------------------*/
int main(void){
    // 测试出块时间，使用Enron Email数据集
    std::string filename = "../../dataset/dataset1.txt";
    experiment exp(filename);
    exp.test_mining(150);

    // // 测试出块时间，使用GPLUS数据集
    // std::string filename = "../../dataset/dataset2.txt";
    // experiment exp(filename);
    // exp.test_mining(500);
}