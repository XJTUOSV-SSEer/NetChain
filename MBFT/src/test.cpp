#include "../include/MBF.h"
#include <cassert>
#include <iostream>
#include "../include/Structs.h"
#include "../include/Query.h"
#include "../include/Block.h"


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
int main(void){
    std::cout<<"-----------------------test-----------------------------"<<std::endl;

    /*-------------------------- 数据集 -------------------------------------*/
    std::vector<transaction> transactions;
    
    transactions.push_back(transaction("a", "c", "friend", 18));
    transactions.push_back(transaction("c", "d", "friend", 20));
    transactions.push_back(transaction("a", "b", "friend", 20));
    transactions.push_back(transaction("a", "d", "friend", 10));
    transactions.push_back(transaction("c", "e", "friend", 9));
    transactions.push_back(transaction("a", "b", "family", 30));
    transactions.push_back(transaction("a", "e", "family", 20));
    transactions.push_back(transaction("b", "e", "family", 40));
    transactions.push_back(transaction("a", "e", "colleague", 3));
    transactions.push_back(transaction("e", "b", "colleague", 10));
    transactions.push_back(transaction("b", "d", "colleague", 18));
    transactions.push_back(transaction("a", "d", "colleague", 8));
    transactions.push_back(transaction("d", "a", "colleague", 9));
    transactions.push_back(transaction("d", "e", "colleague", 7));
    

    

    /*-------------------------- 出块 ----------------------------------------*/
    std::vector<Block> blockchain = Block::construct_chain(transactions, 3, 16, 3, 4);
    

    /*--------------------------- 查询 ----------------------------------------*/
    std::set<std::string> w_q = {"a", "friend"};
    int K=1;
    int lb = 0;
    int ub = 0;
    double alpha = 0;
    double beta = 10000000;

    std::map<size_t, std::vector<MBFT_Node>> VO = Query::MaxSearch(w_q, alpha, beta, lb, ub, blockchain);
    MBFT_Node target_node = Query::MaxVerify(w_q, alpha, beta, lb, ub, blockchain,  VO);

    for(std::string kw : target_node.w_set){
        std::cout << kw << " ";
    }
    std::cout << target_node.v << " " << target_node.l << std::endl;
}