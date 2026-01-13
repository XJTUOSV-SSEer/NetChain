#ifndef QUERY_H
#define QUERY_H

#include <vector>
#include <map>
#include "./MBFT.h"
#include "./Block.h"

class Query{
public:
    /*
        在一个时间窗口的区块内进行MAX搜索，即，寻找包含w_q且数值在[alpha, beta]且数值最大的交易。
        具体地，采用文中的value pruning策略：若第一个区块中获取的最大值为x，则将数值约束更新为[x+1, beta]

        input:
            lb, ub - 区块块号的范围
            chain - 区块链
        output:
            每个区块的块号->区块的证明
    */
    static std::map<size_t, std::vector<MBFT_Node>> MaxSearch(std::set<std::string>& w_q, double alpha, double beta,
                                                                size_t lb, size_t ub, std::vector<Block>& chain);

    /*
        验证并返回匹配对象的指针。若指针为nullptr，则说明不存在匹配结果。
    */
    static MBFT_Node* MaxVerify(std::set<std::string>& w_q, double alpha, double beta,
                                                                size_t lb, size_t ub, std::vector<Block>& chain, std::map<size_t, std::vector<MBFT_Node>>& VO);

    /*
        给定VO对应的子树，验证查询是否正确执行，并重新构建根哈希。递归地构建

        input:
            current_idx - 当前结点在VO子树中的下标
            target_idx - 匹配查询的结点在VO子树中的下标
    */
    static void reconstruct(size_t current_idx, std::set<std::string> w_q, double alpha, 
                            double beta, std::vector<MBFT_Node>& VO_blk);


    /*
        对VO中的结果子树进行DFS。
    */
    static bool dfs(size_t current_idx, std::set<std::string> w_q, double alpha, double beta, 
                    std::vector<MBFT_Node>& VO_blk, int& target_idx);


    /*
        在一个时间窗口的区块内进行top-k搜索，即，寻找包含w_q且数值在[alpha, beta]且数值最大的的k个交易。
        具体地，分为k轮查询。第一轮查询查找最大值m1，第二轮时将数值范围约束修改为[alpha, m1-1]，以此类推。
        每轮结束时验证（偷懒了，将查询和验证合一块了）

        input:
            lb, ub - 区块块号的范围
            chain - 区块链
            K - 参数k
        output:
            一个map，映射轮次->该轮次的所有证明
    */
    static std::map<size_t, std::map<size_t, std::vector<MBFT_Node>>> TopKSearch(std::set<std::string>& w_q, double alpha, double beta,
                                                                size_t lb, size_t ub, std::vector<Block>& chain, size_t K);

};

#endif