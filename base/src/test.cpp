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
int main(void){
    // std::string source = "../../dataset/Email-Enron.txt";
    // std::string target = "../../dataset/dataset1.txt";
    // experiment::adjust_dataset(source, 8, 200, target);

    std::string source = "../../dataset/gplus_combined.txt";
    std::string target = "../../dataset/dataset2.txt";
    // 过滤重复的edge
    // experiment::filterate("../../dataset/gplus_combined.txt");

    // 输出数据集的统计信息
    // experiment::show_dataset("../../dataset/gplus_combined.txt", 1);

    // 调整数据集
    experiment::adjust_dataset(source, 20, 100000000, target);
}



/*------------------------------- 功能测试 ---------------------------------------*/
// int main(void){
//     std::cout<<"-----------------------test-----------------------------"<<std::endl;

//     /*-------------------------- 数据集 -------------------------------------*/
//     std::vector<transaction> transactions;
    
//     transactions.push_back(transaction("a", "c", "friend", 18));
//     transactions.push_back(transaction("a", "b", "friend", 10));
//     transactions.push_back(transaction("a", "d", "friend", 10));
//     transactions.push_back(transaction("c", "d", "friend", 20));
//     transactions.push_back(transaction("c", "e", "friend", 9));
//     transactions.push_back(transaction("a", "b", "family", 30));
//     transactions.push_back(transaction("a", "e", "family", 20));
//     transactions.push_back(transaction("b", "e", "family", 40));
//     transactions.push_back(transaction("a", "e", "colleague", 3));
//     transactions.push_back(transaction("e", "b", "colleague", 10));
//     transactions.push_back(transaction("b", "d", "colleague", 18));
//     transactions.push_back(transaction("a", "d", "colleague", 8));
//     transactions.push_back(transaction("d", "a", "colleague", 9));
//     transactions.push_back(transaction("d", "e", "colleague", 7));
    

    

//     /*-------------------------- 出块 ----------------------------------------*/
//     std::vector<Block> blockchain = Block::construct_chain(transactions, 2);
    

//     /*--------------------------- 查询 ----------------------------------------*/
//     std::string u_q = "d";
//     std::string type_q = "colleague";
//     int K=1;
//     int lb = 0;
//     int ub = 6;

//     Response response = Query::Search(u_q, type_q, K, lb, ub, blockchain);
//     std::vector<std::pair<std::string, int>> final_result= Query::Verify(u_q, type_q, response, K, lb, ub, blockchain);

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
    
    
//     /*--------------------------- 查询 ----------------------------------------*/
//     std::string u_q = "116807883656585676940";
//     std::string type_q = "friend";
//     int K=100;
//     int lb = 0;
//     int ub = blockchain.size()-1;
//     auto end = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double> duration = end - start;
//     // 输出执行时间
//     std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    

    
//     Response response = Query::Search(u_q, type_q, K, lb, ub, blockchain);
//     std::vector<std::pair<std::string, int>> final_result= Query::Verify(u_q, type_q, response, K, lb, ub, blockchain);
    
    
    
//     for(int i=0; i<final_result.size(); i++){
//         std::cout << final_result[i].first << "," << final_result[i].second <<std::endl;
//     }
// }




















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
