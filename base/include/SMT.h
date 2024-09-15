#ifndef SMT_H
#define SMT_H

#include <vector>
#include <string>
#include <set>
#include "Crypto_Primitives.h"
#define DIGEST_LENGHT 32


// SMT的结点
struct SMTNode{
    // 标识是否为叶结点
    bool isLeaf;
    // 混合键 <u, type>
    std::pair<std::string, std::string> compound_key;
    // 该混合键对应sorted list的长度
    int l;
    // 该混合键对应sorted list的头哈希
    std::string h1;
    // 左子结点在tree数组中的下标。若不存在，置为-1
    int lchild;
    // 右子结点在tree数组中的下标。若不存在，置为-1
    int rchild;
    // 结点的哈希, 叶结点的哈希为H(u||type||l||h1)，非叶结点的哈希为H(lhash||rhash)
    std::string digest;

    // 构造函数，构造一个叶结点
    SMTNode(std::string u, std::string type, int l, std::string h1){
        this->isLeaf = true;
        this->compound_key = std::make_pair(u, type);
        this->l = l;
        this->h1 = h1;

        // H(u||type||l||h1)
        std::string msg = u+type+ std::to_string(l) + h1;
        this->digest = Crypto_Primitives::SHA256_digest(msg);

        this->lchild = -1;
        this->rchild = -1;
    }

    // 构造函数，构造一个非叶结点
    SMTNode(int lchild, int rchild, std::string lhash, std::string rhash){
        this->isLeaf = false;

        this->lchild = lchild;
        this->rchild = rchild;
        
        // H(lhash||rhash)
        this->digest = Crypto_Primitives::SHA256_digest(lhash+rhash);
    }
};


/*
    使用递归，构造平衡的SMT
*/
class SMT{
public:
    // 使用数组储存SMT。由于递归的构造方式，最后一个元素必定是树根
    std::vector<struct SMTNode> tree;
    // 叶结点数量
    int leaf_num;
    // 根结点在tree数组中的下标
    int root_id;


    /*
        默认构造函数
    */
    SMT();
    
    /*
        构造函数
        param:
            leaves - 一个数组，储存所有叶结点

        return:
    */
    SMT(std::vector<struct SMTNode> leaves);


    /*
        根据叶结点，递归地构造平衡SMT。
        递归算法：将当前的叶结点分为大小相等的两个子树，从而得到lhash和rhash，之后计算当前结点的digest并返回。

        param:
            lb - 当前结点所在子树对应的lower bound叶结点的下标
            ub - 当前结点所在子树对应的upper bound叶结点的下标

        return:
            当前子树根结点在tree数组中的下标
    */
    int construct_tree(int lb, int ub);
};



#endif