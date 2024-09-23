#include <iostream>
#include <vector>
#include "../include/SMT.h"
#include "../include/Block.h"
#include "../include/SMT.h"
#include "../include/Structs.h"
#include "../include/Query.h"
#include "../include/experiment.h"
#include <chrono>


/*-------------------------------- 调整数据集 --------------------------------------*/ 
// int main(void){
//     std::string source = "../../dataset/Email-Enron.txt";
//     std::string target = "../../dataset/dataset1.txt";
//     experiment::adjust_dataset(source, 8, 200, target);
// }



/*------------------------------- 功能测试 ---------------------------------------*/
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
//     transactions.push_back(transaction("1", "4", "friend", 10));
    
    
    

    

//     /*-------------------------- 出块 ----------------------------------------*/
//     MPT mpt;
//     std::vector<Block> blockchain = Block::construct_chain(transactions, 2, mpt);
    

//     /*--------------------------- 查询 ----------------------------------------*/
//     std::string u_q = "1";
//     std::string type_q = "colleague";
//     int K=2;
//     int lb = 5;
//     int ub = 6;

//     Response response = Query::Search(u_q, type_q, K, lb, ub, blockchain, mpt);
//     std::vector<std::pair<std::string, int>> final_result= Query::Verify(u_q, type_q, response, K, lb, ub, blockchain);

//     for(int i=0; i<final_result.size(); i++){
//         std::cout << final_result[i].first << "," << final_result[i].second <<std::endl;
//     }
// }





/*-------------------------------- 大数据集测试 --------------------------------------*/
int main(void){
    std::string filename = "../../dataset/dataset1.txt";
    experiment exp(filename);


    std::vector<transaction>& transactions = exp.transactions;

    /*-------------------------- 出块 ----------------------------------------*/
    auto start = std::chrono::high_resolution_clock::now();
    MPT mpt;
    std::vector<Block> blockchain = Block::construct_chain(transactions, 10, mpt);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    // 输出执行时间
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    

    /*--------------------------- 查询 ----------------------------------------*/
    std::string u_q = "11110";
    std::string type_q = "friend";
    int K=10;
    int lb = 0;
    int ub = blockchain.size()-1;
    // int ub = 31672;

    
    Response response = Query::Search(u_q, type_q, K, lb, ub, blockchain, mpt);
    std::vector<std::pair<std::string, int>> final_result= Query::Verify(u_q, type_q, response, K, lb, ub, blockchain);
    
    
    
    for(int i=0; i<final_result.size(); i++){
        std::cout << final_result[i].first << "," << final_result[i].second <<std::endl;
    }
}



















// int main(){
//     std::cout<<"-----------------------test-----------------------------"<<std::endl;
    
//     // 设置数据集
//     std::vector<transaction> transactions;
//     transactions.push_back(transaction("Alice", "Bob", "friend", 3));
//     transactions.push_back(transaction("Alice", "Lois", "friend", 4));
//     transactions.push_back(transaction("Peter", "Lois", "family", 7));
//     transactions.push_back(transaction("Alice", "Peter", "family", 7));

//     // 出块
//     Block blk1(1, std::string(32, '\0'), transactions);
//     std::cout << blk1.blk_id<<std::endl;
//     // std::cout << blk1.pre_hash<<std::endl;
//     // std::cout << blk1.h_smt.length()<<std::endl;
//     // std::cout << blk1.smt.root_id<<std::endl;
//     std::vector<SMTNode> tree = blk1.smt.tree;
//     for(int i=0; i<blk1.smt.leaf_num; i++){
//         std::cout << tree[i].compound_key.first<< "," << tree[i].compound_key.second <<std::endl;

//     }
// }
