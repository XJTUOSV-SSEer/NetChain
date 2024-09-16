#include <iostream>
#include "../include/Block.h"
#include <vector>
#include "../include/SMT.h"


int main(){
    std::cout<<"-----------------------test-----------------------------"<<std::endl;
    
    // 设置数据集
    std::vector<transaction> transactions;
    transactions.push_back(transaction("Alice", "Bob", "friend", 3));
    transactions.push_back(transaction("Alice", "Lois", "friend", 4));
    transactions.push_back(transaction("Peter", "Lois", "family", 7));
    transactions.push_back(transaction("Alice", "Peter", "family", 7));

    // 出块
    Block blk1(1, std::string(32, '\0'), transactions);
    std::cout << blk1.blk_id<<std::endl;
    // std::cout << blk1.pre_hash<<std::endl;
    // std::cout << blk1.h_smt.length()<<std::endl;
    // std::cout << blk1.smt.root_id<<std::endl;
    std::vector<SMTNode> tree = blk1.smt.tree;
    for(int i=0; i<blk1.smt.leaf_num; i++){
        std::cout << tree[i].compound_key.first<< "," << tree[i].compound_key.second <<std::endl;
    }
}
