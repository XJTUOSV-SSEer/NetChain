#include "../include/SMT.h"
#include <algorithm>

SMT::SMT(){

}


bool compare_by_compound_key(const SMTNode& a, const SMTNode& b){
    return a.compound_key < b.compound_key;
}


SMT::SMT(std::vector<struct SMTNode> leaves){
    this->leaf_num = leaves.size();

    // 将leaves中所有叶结点加入tree数组，并根据混合键对元素进行排序
    // std::copy(leaves.begin(), leaves.end(), std::back_inserter(this->tree));
    this->tree = leaves;
    std::sort(this->tree.begin(), this->tree.end(), compare_by_compound_key);

    // 构造SMT，得到根结点
    int root_id = construct_tree(0, this->leaf_num-1);
    this->root_id = root_id;
}



int SMT::construct_tree(int lb, int ub){
    // 递归边界：当子树中只有一个叶结点时，直接返回
    if(lb==ub){
        return lb;
    }

    // 当子树中有至少两个叶结点时，可以构造高度至少为2的子树
    // 左子树
    int lchild = construct_tree(lb, int((lb+ub)/2));
    std::string lhash = tree[lchild].digest;
    // 右子树
    int rchild = construct_tree(int((lb+ub)/2)+1, ub);
    std::string rhash = tree[rchild].digest;

    // 当前子树的根结点
    struct SMTNode root = SMTNode(lchild, rchild, lhash, rhash);
    (this->tree).push_back(root);

    // 返回root在tree数组中的下标
    return (this->tree).size()-1;
}