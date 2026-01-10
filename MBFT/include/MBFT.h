#ifndef MBFT_H
#define MBFT_H
#include <vector>
#include <set>
#include <string>
#include "./MBF.h"
#include "./Structs.h"


/* MBFT的结点 */
// 这些用继承和多态没用对，有空了用智能指针重写
// class MBFT_Node{
// public:           
//     std::set<std::string> w_set;        // 储存交易中的u, type两个关键字
//     MBF mbf;
//     double l;
//     double u;
//     std::string digest;                 // 哈希摘要

//     /*
//         构造函数
//     */
//     MBFT_Node(const std::set<std::string>& w_set, double l, double u, size_t m_, size_t k_, size_t len_h_);

//     /*
//         计算此结点的哈希摘要，
//     */
//     virtual std::string cal_digest(std::string lhash, std::string rhash);

//     virtual ~MBFT_Node() = default;

//     /*
//         将 double 转换为字节数组（即其内存表示），再封装为 std::string
//     */
//     static std::string double_to_string_bytes(double value);
// };


class MBFT_Node{
public:           
    bool is_leaf;
    std::set<std::string> w_set;        // 储存交易中的u, type两个关键字
    MBF mbf;
    std::string v;                      // 储存交易中的关键字v
    double l;
    double u;
    int lchild;      // 左子结点在tree数组中的下标
    int rchild;      // 右子结点在tree数组中的下标
    std::vector<bool> hint;         // hint，用于后续验证
    std::string digest;                 // 哈希摘要
    std::string lhash;                  // 左子结点的哈希摘要，认证时需要加入VO
    std::string rhash;                  // 右子结点的哈希摘要，认证时需要加入VO

    /*
        构造叶结点
    */
    MBFT_Node(const std::set<std::string>& w_set, double l, double u, std::string v, size_t m_, size_t k_, size_t len_h_);

    /*
        构造内部结点
    */
    MBFT_Node(const std::set<std::string>& w_set, double l, double u, std::string lhash, std::string rhash, size_t m_, size_t k_, 
                size_t len_h_, int lchild, int rchild);

    /*
        计算叶结点的哈希摘要，
    */
    std::string cal_digest();

    /*
        计算内部结点的哈希摘要，
    */
    std::string cal_digest(std::string lhash, std::string rhash);

    ~MBFT_Node() = default;

    /*
        将 double 转换为字节数组（即其内存表示），再封装为 std::string
    */
    static std::string double_to_string_bytes(double value);
};


/*
    叶结点子类
*/
// class LeafNode : public MBFT_Node {
// public:
//     std::string v;                      // 储存交易中的关键字v

//     LeafNode(const std::set<std::string>& w_set, double l, double u, std::string v, size_t m_, size_t k_, size_t len_h_);

//     /*
//         定义为digest=H(mbf||l||u||v)
//     */
//     std::string cal_digest(std::string lhash, std::string rhash) override;
// };

/*
    内部结点子类
*/
// class InternalNode : public MBFT_Node{
// public:
//     size_t lchild;      // 左子结点在tree数组中的下标
//     size_t rchild;      // 右子结点在tree数组中的下标
//     std::vector<bool> hint;         // hint，用于后续验证
//     InternalNode(const std::set<std::string>& w_set, double l, double u, std::string lhash, std::string rhash, size_t m_, size_t k_, size_t len_h_, size_t lchild, size_t rchild);

//     /*
//         定义为digest=H(lhash||rhash||mbf||l||u)
//     */
//     std::string cal_digest(std::string lhash, std::string rhash) override;
// };



class MBFT{
public:
    std::vector<MBFT_Node> tree;        // 静态方式储存树，根结点在数组末尾

    MBFT();

    /*
        给定一个区块内的交易，构造一棵MBFT
        具体地，自底向上地构建。若某一层的结点个数为大于1的奇数，则补充一个空结点（关键字集合为NULL，MBF为全0）
        叶结点中MBF对应的朴素布隆过滤器长度设置为16（因为误差率设置为0.05）

        input:
            original_m_ - 叶结点中MBF的参数m_
            original_k_ - 叶结点中MBF的参数k_
            original_len_h_ - 叶结点中MBF的参数len_h_
    */
    MBFT(std::vector<transaction>& txs, size_t original_m_, size_t original_k_, size_t original_len_h_);



    /*
        给定一个MAX查询Q（数值属性应当满足[alpha, beta]），对区块中的MBFT进行搜索并认证，返回VO

        input:
            w_q - 被查询的关键字，要求匹配的对象中包含w_q中所有关键字
            alpha - 数值范围的下界
            beta - 数值范围的上界
            max_value - 匹配交易的数值
    */
    std::vector<MBFT_Node> retrieve(std::set<std::string>& w_q, double alpha, double beta, double& max_value);


    /*
        深度优先搜索算法，递归写法
        input:
            current_idx - 当前访问的结点在tree数组中的下标
            VO - 验证对象（一棵子树）
        output:
            当前node是否on path
    */
    bool dfs(size_t current_idx, std::set<std::string> w_q, double alpha, double beta, std::vector<MBFT_Node>& VO, double& max_value);


};



#endif