#ifndef SMT_H
#define SMT_H

#include <vector>
#include <string>
#include <set>
#include "Crypto_Primitives.h"
#include "Structs.h"
#define DIGEST_LENGHT 32





/*
    使用递归，构造平衡的SMT
*/
class SMT{
private:
    /*
        给定若干叶结点，生成这些叶结点对应的merkle proof（本质是一棵子树）
        具体算法：通过递归，判断当前结点的子结点是否on path（在leaves中结点到根结点的路径上）。若某个子结点
        on path，则当前结点也on path，需要加入子树proof.subtree。
        param:
            leaves - 若干叶结点在SMT.tree数组中的下标
            id - 当前结点在SMT.tree数组中的下标
            proof - 证明
        return:
            若当前结点on path，返回当前结点在proof.subtree数组中的下标；否则返回-1
    */
    int merkle_proof(std::set<int>& leaves, int id, SMTProof& proof);

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
    SMT(std::vector<struct SMTNode>& leaves);


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


    /*
        证明不存在性
        param: 
            com_key_q - 要查询的混合键
        return:
            不存在证明
    */
    SMTProof prove_Nonexistence(std::pair<std::string, std::string> com_key_q);


    /*
        验证不存在性
    */
    bool verify_Nonexistence(SMTProof proof);
};



#endif