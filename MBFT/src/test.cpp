#include "../include/MBF.h"
#include <cassert>
#include <iostream>
#include "../include/Structs.h"
#include "../include/Query.h"
#include "../include/Block.h"
#include <fstream>
#include <sstream>
#include <random>


// 测试MBF add
// int main(){
//     size_t m = 8, k = 3;
//     MBF mbf(m, k);

//     std::set<std::string> s = {"alice", "bob"};
//     mbf.add(s);
//     assert(mbf.check("alice") == true);
//     assert(mbf.check("bob") == true);
//     assert(mbf.check("abc") == true);

//     std::cout << "PASS\n";
// }

// // 大数据量测试add和check
// int main(void){
//     size_t expected_elements_ = 1000;
//     size_t m = 4096, k = 3;
//     MBF mbf(m, k);
//     std::set<std::string> w_set;
//     for(size_t i=1; i<=expected_elements_; i++) {
//         w_set.insert(std::to_string(i));
//     }
//     mbf.add(w_set);
//     for(size_t i=1; i<=expected_elements_; i++) {
//         assert(mbf.check(std::to_string(i))==true);
//     }

//     std::cout << "PASS\n";
// }

// int main(){
//     size_t m = 4, k = 3;

//     MBF mbf1(m, k);
//     MBF mbf2(m, k);
//     std::set<std::string> s1 = {"a", "b", "e"};
//     std::set<std::string> s2 = {"c", "d", "e"};
//     mbf1.add(s1);
//     mbf2.add(s2);

//     MBF mbf3(2*m, k);
//     std::set<std::string> s3 = {"a", "b", "c", "d", "e"};
//     mbf3.add(s3);
//     std::vector<bool> hint = mbf3.gen_hint();

//     MBF mbf4(2*m, k);
//     mbf4.merge(mbf1, mbf2, hint);        // 检查合并后的MBF是否正确
//     assert(mbf3.m_ == mbf4.m_);
//     for(size_t i=0; i<mbf3.m_; i++){
//         assert(mbf3.bits_[i] == mbf4.bits_[i]);
//     }
// }



// 大数据量测试merge
// int main(void){
//     size_t expected_elements_ = 1000;
//     size_t m = 4096, k = 3;
//     MBF mbf1(m, k);
//     MBF mbf2(m, k);
//     std::set<std::string> w_set1;
//     std::set<std::string> w_set2;
//     std::set<std::string> w_set3;
//     for(size_t i=1; i<=expected_elements_; i++) {
//         w_set1.insert(std::to_string(i));
//         w_set2.insert(std::to_string(i+500));
//         w_set3.insert(std::to_string(i));
//         w_set3.insert(std::to_string(i+500));
//     }
//     mbf1.add(w_set1);
//     mbf2.add(w_set2);
    
//     MBF mbf3(2*m, k);
//     mbf3.add(w_set3);

//     std::vector<bool> hint = mbf3.gen_hint();

//     MBF mbf4(2*m, k);
//     mbf4.merge(mbf1, mbf2, hint);        // 检查合并后的MBF是否正确
    

//     for(size_t i=0; i<mbf3.bits_.size(); i++){
//         assert(mbf3.bits_[i] == mbf4.bits_[i]);
//     }
//     std::cout << "PASS\n";
// }




/*------------------------------- MaxSearch功能测试 ---------------------------------------*/
// int main(void){
//     std::cout<<"-----------------------test-----------------------------"<<std::endl;

//     /*-------------------------- 数据集 -------------------------------------*/
//     std::vector<transaction> transactions;
    
//     transactions.push_back(transaction("a", "c", "friend", 18));
//     transactions.push_back(transaction("c", "d", "friend", 20));
//     transactions.push_back(transaction("a", "b", "friend", 20));
//     transactions.push_back(transaction("a", "d", "friend", 10));
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
//     std::vector<Block> blockchain = Block::construct_chain(transactions, 6, 16, 3, 4);
    
//     /*--------------------------- 查询 ----------------------------------------*/
//     std::set<std::string> w_q = {"a", "friend"};
//     int lb = 0;
//     int ub = 2;
//     double alpha = 0;
//     double beta = 10000000;

//     std::map<size_t, std::vector<MBFT_Node>> VO = Query::MaxSearch(w_q, alpha, beta, lb, ub, blockchain);
//     MBFT_Node target_node = Query::MaxVerify(w_q, alpha, beta, lb, ub, blockchain,  VO);

//     for(std::string kw : target_node.w_set){
//         std::cout << kw << " ";
//     }
//     std::cout << target_node.v << " " << target_node.l << std::endl;
// }





/*-------------------------- MAXSearch多用例测试 -----------------------------------*/
void gen_data(){
    std::string filename = "../test_data.txt";
    std::ofstream f(filename);

    std::vector<std::string> types = {"friend", "family", "colleague"};
    for(size_t i = 0; i < 10000; i++){
        std::string u = std::to_string((rand() % 100)+1);
        std::string v = std::to_string((rand() % 100)+1);
        std::string type = types[rand() % types.size()];
        int w = rand() % 5000;

        // 交易参数写入文件
        f << u << " " << v << " " << type << " " << w << "\n";
    }
}

int main(){
    // 生成数据
    // gen_data();

    // 读取数据，构造交易
    std::vector<transaction> transactions;
    std::string filename = "../test_data.txt";
    std::ifstream f(filename);
    std::string line;
    while (std::getline(f, line)) {
        std::stringstream ss(line);
        std::string u, v, type;
        int w;
        ss >> u >> v >> type >> w;
        transactions.push_back(transaction(u, v, type, w));
    }

    /*-------------------------- 测试不同区块大小下的情况 -----------------------------------*/
    for(size_t max_transactions = 1; max_transactions <= 1000; max_transactions ++){
        std::vector<Block> blockchain = Block::construct_chain(transactions, max_transactions, 16, 3, 4);

        std::vector<std::string> types = {"friend", "family", "colleague"};
        // 查询，遍历所有可能的<u,type>        
        for(size_t i = 1; i<=100; i++){
            for(std::string type : types){
                std::cout << "queried: " << std::to_string(i) << " " << type <<std::endl;
                std::set<std::string> w_q;
                w_q.insert(std::to_string(i));
                w_q.insert(type);
                int lb = 0;
                int ub = blockchain.size()-1;
                double alpha = 0;
                double beta = 10000000;
                std::map<size_t, std::vector<MBFT_Node>> VO = Query::MaxSearch(w_q, alpha, beta, lb, ub, blockchain);
                MBFT_Node* target_node = Query::MaxVerify(w_q, alpha, beta, lb, ub, blockchain,  VO);

                // 检查结果是否正确
                if(target_node == nullptr){
                    // 确认不存在满足的交易
                    for(transaction tx : transactions){
                        assert(tx.u != std::to_string(i) || tx.type != type);
                    }
                }
                else{
                    // 找到正确的匹配对象并对比
                    double max_value = -1;
                    for(transaction tx : transactions){
                        if(tx.u == std::to_string(i) && tx.type == type){
                            max_value = std::max(max_value, tx.w);
                        }
                    }
                    assert(max_value == target_node->l);
                    // if((double)max_value != target_node->l){
                    //     // 检查是否发生了假阳性
                    //     for(std::string _w : w_q){
                    //         assert(target_node->mbf.check(_w));
                    //     }                        
                    // }
                }
            }
        }   

    }

}