#include "../include/MBFT.h"
#include "../include/MBF.h"
#include "../include/Crypto_Primitives.h"
#include <algorithm>
#include <queue>


std::string MBFT_Node::double_to_string_bytes(double value){
    std::string result(8, '\0'); // 构造 8 字节字符串，初始化为 0
    std::memcpy(&result[0], &value, sizeof(double));
    return result;
}



// MBFT_Node::MBFT_Node(const std::set<std::string>& w_set, double l, double u, size_t m_, size_t k_, size_t len_h_):
// w_set(w_set), l(l), u(u), mbf(m_, k_, len_h_)
// {
//     mbf.add(w_set);
// }


// std::string LeafNode::cal_digest(std::string lhash, std::string rhash){
//     std::string mbf_bytes = mbf.bin_form();
//     std::string l_bytes = double_to_string_bytes(l);
//     std::string r_bytes = double_to_string_bytes(u);
//     return Crypto_Primitives::SHA256_digest(mbf_bytes+l_bytes+r_bytes+this->v);
// }

// std::string InternalNode::cal_digest(std::string lhash, std::string rhash){
//     std::string mbf_bytes = mbf.bin_form();
//     std::string l_bytes = double_to_string_bytes(l);
//     std::string r_bytes = double_to_string_bytes(u);
//     return Crypto_Primitives::SHA256_digest(lhash+rhash+mbf_bytes+l_bytes+r_bytes);
// }


// LeafNode::LeafNode(const std::set<std::string>& w_set, double l, double u, std::string v, size_t m_, size_t k_, size_t len_h_):
// MBFT_Node(w_set, l, u, m_, k_, len_h_)    {
//     this->v = v;
//     this->digest = this->cal_digest("1", "1");
// }


// InternalNode::InternalNode(const std::set<std::string>& w_set, double l, double u, std::string lhash, std::string rhash, size_t m_, size_t k_, size_t len_h_, size_t lchild, size_t rchild):
// MBFT_Node(w_set, l, u, m_, k_, len_h_) {
//     this->lchild = lchild;
//     this->rchild = rchild;
//     this->digest = this->cal_digest(lhash, rhash);
// }




MBFT_Node::MBFT_Node(const std::set<std::string>& w_set, double l, double u, std::string v, size_t m_, size_t k_, size_t len_h_)
:w_set(w_set), l(l), u(u), mbf(m_, k_, len_h_)
{
    mbf.add(w_set);
    this->is_leaf = true;
    this->v = v;
    this->digest = this->cal_digest();    
}


MBFT_Node::MBFT_Node(const std::set<std::string>& w_set, double l, double u, std::string lhash, std::string rhash, size_t m_, size_t k_, 
                size_t len_h_, int lchild, int rchild) : w_set(w_set), l(l), u(u), mbf(m_, k_, len_h_)
{
    mbf.add(w_set);
    this->is_leaf = false;
    this->lchild = lchild;
    this->rchild = rchild;
    this->lhash = lhash;
    this->rhash = rhash;
    this->digest = this->cal_digest(lhash, rhash);    
}


std::string MBFT_Node::cal_digest(){
    std::string mbf_bytes = mbf.bin_form();
    std::string l_bytes = double_to_string_bytes(l);
    std::string r_bytes = double_to_string_bytes(u);
    return Crypto_Primitives::SHA256_digest(mbf_bytes+l_bytes+r_bytes+this->v);
}


std::string MBFT_Node::cal_digest(std::string lhash, std::string rhash){
    std::string mbf_bytes = mbf.bin_form();
    std::string l_bytes = double_to_string_bytes(l);
    std::string r_bytes = double_to_string_bytes(u);
    return Crypto_Primitives::SHA256_digest(lhash+rhash+mbf_bytes+l_bytes+r_bytes);
}





MBFT::MBFT(std::vector<transaction>& txs, size_t original_m_, size_t original_k_, size_t original_len_h_){
    std::vector<MBFT_Node> former_level;        // 储存之前生成的一层的结点
    std::vector<MBFT_Node> current_level;        // 储存当前层的结点

    // 将交易按照权重值升序排序
    std::sort(txs.begin(), txs.end());

    // 将每个交易转换为一个leaf node，关键字集合选用交易中的u, type字段
    for(transaction& tx : txs){
        std::set<std::string> w_set;
        w_set.insert(tx.u);
        w_set.insert(tx.type);
        MBFT_Node leaf(w_set, tx.w, tx.w, tx.v, original_m_, original_k_, original_len_h_);     // 构造叶结点
        current_level.push_back(leaf);         // 叶结点加入current_level
    }

    // 若叶结点个数为大于1的奇数，加入一个空结点
    if(current_level.size() > 1 && current_level.size() % 2 == 1){
        std::set<std::string> w_set;
        MBFT_Node leaf(w_set, 0, 0, "0", original_m_, original_k_, original_len_h_);
        current_level.push_back(leaf);
    }

    // 使用队列构造上层的非叶结点
    size_t current_m_ = original_m_ * 2;
    size_t current_len_h = original_len_h_ + 1;
    while(current_level.size() > 1){
        former_level = current_level;
        current_level.clear();

        // 对下层的结点，每两个构造一个父结点
        int j = 0;          // 标识former_level的当前扫描位置
        while(j < former_level.size()){
            MBFT_Node& lcnode = former_level[j];        // 左子结点
            MBFT_Node& rcnode = former_level[j+1];        // 右子结点
            // 将这两个子结点加入tree数组
            this->tree.push_back(lcnode);
            this->tree.push_back(rcnode);

            // 对子结点的关键字求并集
            std::set<std::string> new_w_set;
            std::set_union(lcnode.w_set.begin(), lcnode.w_set.end(),
                    rcnode.w_set.begin(), rcnode.w_set.end(),
                    std::inserter(new_w_set, new_w_set.begin()));
            
            // 计算l,u。l=min(l_l, l_r), u=max(u_l, u_r)
            // 特殊情况下，一个子结点为空结点，则直接使用另一个非空子结点的参数
            double new_l, new_u;
            if(rcnode.w_set.empty()){
                new_l = lcnode.l;
                new_u = lcnode.u;
            }
            else{
                new_l = std::min(lcnode.l, rcnode.l);
                new_u = std::max(lcnode.u, rcnode.u);
            }

            // 构造父结点
            MBFT_Node parent_node(new_w_set, new_l, new_u, lcnode.digest, rcnode.digest, current_m_, original_k_, current_len_h, 
                        tree.size()-2, tree.size()-1);
            current_level.push_back(parent_node);

            j += 2;
        }

        // 若该层结点数为大于1的奇数，加入一个空结点
        if(current_level.size() > 1 && current_level.size() % 2 == 1){
            std::set<std::string> w_set;
            MBFT_Node leaf(w_set, 0, 0, "0", current_m_, original_k_, current_len_h);
            current_level.push_back(leaf);
        }

        // 更新参数
        current_m_ = current_m_ * 2;
        current_len_h = current_len_h + 1;
    }

    // 将根结点加入tree
    tree.push_back(current_level[0]);
}




std::vector<MBFT_Node> MBFT::retrieve(std::set<std::string>& w_q, double alpha, double beta, double& max_value){
    std::vector<MBFT_Node> VO;
    // 调用DFS，从根结点开始
    dfs(tree.size()-1, w_q, alpha, beta, VO, max_value);
    return VO;
}



bool MBFT::dfs(size_t current_idx, std::set<std::string> w_q, double alpha, double beta, std::vector<MBFT_Node>& VO, double& max_value){
    // 判断当前结点是否满足查询条件
    MBFT_Node current_node = tree[current_idx];
    bool is_match = true;
    // 检查关键字条件
    for(std::string kw : w_q){
        if(!current_node.mbf.check(kw)){
            is_match = false;
            break;
        }
    }
    // 若MBF检查通过且当前结点为叶结点，进一步检查关键字集合，避免假阳性
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


    // 递归边界：若满足条件且是叶结点，将完整MBF加入VO并返回true
    if(is_match && current_node.is_leaf){
        VO.push_back(current_node);
        max_value = current_node.l;
        return true;
    }

    // 若当前结点满足条件且是内部结点，则递归检查子结点
    if(is_match){
        // 检查右子结点
        if(dfs(current_node.rchild, w_q, alpha, beta, VO, max_value)){
            // 右子结点on-path，需要把左子结点也加入VO。这里复用了current_node来加入VO
            VO.push_back(tree[current_node.lchild]);
            current_node.rchild = VO.size() - 2;
            current_node.lchild = VO.size() - 1;
            VO[current_node.lchild].lchild = -1;
            VO[current_node.lchild].rchild = -1;
            current_node.hint = current_node.mbf.gen_hint();
            VO.push_back(current_node);
            return true;
        }
        // 检查左子结点
        else{
            current_node.rchild = VO.size() - 1;
            // 若左子结点on-path
            if(dfs(current_node.lchild, w_q, alpha, beta, VO, max_value)){
                current_node.lchild = VO.size() - 1;
                current_node.hint = current_node.mbf.gen_hint();
                VO.push_back(current_node);
                return true;
            }
            else{
                current_node.lchild = VO.size() - 1;
                current_node.hint = current_node.mbf.gen_hint();
                VO.push_back(current_node);
                return false;
            }
        }        
    }
    // 当前结点不匹配查询条件，将完整结点加入VO
    else{
        // 若当前结点是非叶结点，要额外设置lchild和rchild
        if(!current_node.is_leaf){
            current_node.lhash = tree[current_node.lchild].digest;
            current_node.rhash = tree[current_node.rchild].digest;
            current_node.lchild = -1;
            current_node.rchild = -1;            
        }
        VO.push_back(current_node);
        return false;
    }
}


MBFT::MBFT(){

}