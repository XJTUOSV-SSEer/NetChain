#include "../include/Query.h"
#include <cassert>



std::map<size_t, std::vector<MBFT_Node>> Query::MaxSearch(std::set<std::string>& w_q, double alpha, double beta,
                                                                size_t lb, size_t ub, std::vector<Block>& chain){
    double current_alpha = alpha;
    double current_beta = beta;
    std::map<size_t, std::vector<MBFT_Node>> VO;        // 储存每个区块的VO
    for(size_t i = lb; i <= ub; i++){
        Block& blk = chain[i];

        // 对当前区块的MBFT进行查询
        double max_value = -1;      // 记录区块中匹配交易的数值
        VO[i] = blk.mbft.retrieve(w_q, current_alpha, current_beta, max_value);
        
        // 更新范围参数
        if(max_value >= 0){
            current_alpha = max_value;
        }        
    }

    return VO;
}




MBFT_Node* Query::MaxVerify(std::set<std::string>& w_q, double alpha, double beta,
                                                                size_t lb, size_t ub, std::vector<Block>& chain, std::map<size_t, std::vector<MBFT_Node>>& VO){
    double current_alpha = alpha;
    double current_beta = beta;
    MBFT_Node* target_obj = nullptr;          // 指向全局最大匹配交易

    for(size_t i = lb; i <= ub; i++){
        std::vector<MBFT_Node>& VO_blk = VO[i];
        // 重构根哈希以验证当前区块的proof
        reconstruct(VO_blk.size()-1, w_q, current_alpha, current_beta, VO_blk);
        assert(VO_blk[VO_blk.size()-1].digest == chain[i].h_mbft);

        // 对VO中通过验证的子树，进行DFS并验证结果是否正确
        int target_idx = -1;      // 记录区块中匹配交易的下标
        dfs(VO_blk.size()-1, w_q, current_alpha, current_beta, VO_blk, target_idx);
        // 存在匹配交易，更新alpha
        if(target_idx >= 0){
            target_obj = &(VO_blk[target_idx]);
            current_alpha = target_obj->l;
        }
    }

    // 返回查询结果
    return target_obj;
}



void Query::reconstruct(size_t current_idx, std::set<std::string> w_q, double alpha, double beta, std::vector<MBFT_Node>& VO_blk){
    MBFT_Node& current_node = VO_blk[current_idx];

    // 若当前结点为叶结点，重算哈希并返回
    if(current_node.is_leaf){
        current_node.digest = current_node.cal_digest();
        return;
    }    

    // 若当前结点为非叶结点且lchild=-1 && rchild=-1，重算哈希并返回
    if(current_node.lchild == -1 & current_node.rchild == -1){
        current_node.digest = current_node.cal_digest(current_node.lhash, current_node.rhash);
    }
    // 若当前结点为非叶结点且lchild!=-1 && rchild!=-1，根据hint重建MBF，然后重算哈希并返回
    else{
        // 递归构建左、右子结点
        reconstruct(current_node.lchild, w_q, alpha, beta, VO_blk);
        reconstruct(current_node.rchild, w_q, alpha, beta, VO_blk);
        current_node.mbf.merge(VO_blk[current_node.lchild].mbf, VO_blk[current_node.rchild].mbf, current_node.hint);
        current_node.digest = current_node.cal_digest(VO_blk[current_node.lchild].digest, VO_blk[current_node.rchild].digest);
    }
    return;
}



bool Query::dfs(size_t current_idx, std::set<std::string> w_q, double alpha, double beta, std::vector<MBFT_Node>& VO_blk, int& target_idx){
    // 判断当前结点是否满足查询条件
    MBFT_Node current_node = VO_blk[current_idx];
    bool is_match = true;
    // 检查关键字条件
    for(std::string kw : w_q){
        if(!current_node.mbf.check(kw)){
            is_match = false;
            break;
        }
    }
    // 若MBF检查通过，进一步检查关键字集合，避免假阳性
    if(is_match && current_node.is_leaf) {
        for(std::string kw : w_q) {
            if(current_node.w_set.find(kw) == current_node.w_set.end()){
                is_match = false;
                break;
            }
        }
    }
    // 检查数值条件
    if(is_match){
        double a = std::max(alpha, current_node.l);
        double b = std::min(beta, current_node.u);
        if(a > b){
            is_match = false;
        }
    }

    // 递归边界：若满足条件且是叶结点，返回true
    if(is_match && current_node.is_leaf){
        target_idx = current_idx;
        return true;
    }
    // 若当前结点满足条件且是内部结点，则递归检查子结点
    if(is_match){
        assert(current_node.rchild >= 0 && current_node.lchild >= 0);
        // 检查右子结点
        if(dfs(current_node.rchild, w_q, alpha, beta, VO_blk, target_idx)){
            return true;
        }
        // 检查左子结点
        else{
            // 若左子结点on-path
            if(dfs(current_node.lchild, w_q, alpha, beta, VO_blk, target_idx)){
                return true;
            }
            else{
                return false;
            }
        }        
    }
    else{
        return false;
    }
}
