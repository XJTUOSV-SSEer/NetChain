#include "../include/Query.h"
#include <iostream>




Response Query::search(std::vector<Block>& blockchain, std::string u_q, std::string type_q, int K, 
                        int lb, int ub){
    // 初始化response
    Response response;

    // 混合键
    std::pair<std::string, std::string> com_key_q = std::make_pair(u_q, type_q);
    // MSA
    MultisetAccumulator msa;
    // 混合键对应的素数
    BigInteger p_bi;
    BigInteger::generate_prime(p_bi, com_key_q.first + com_key_q.second);
    std::string p_str = p_bi.to_string();


    // 遍历所有区块
    for(int i=lb; i<=ub; i++){
        // 对应于此区块的MHTProof
        MHTProof mht_proof;

        // 取出区块
        Block& blk = blockchain[i];

        // 取出区块中的MHT
        MHT& mht = blk.mht;

        // 对MHT进行搜索，得到结果和证明
        mht_proof.root_id = mht.search(mht_proof, blk.mht.root_id, com_key_q, msa, p_str);

        // 将证明加入VO
        response.VO[i] = mht_proof;
    }

    // 搜索结束
    return response;
}





std::vector<std::pair<std::string, int>> Query::verify(std::vector<Block>& blockchain, 
                std::string u_q, std::string type_q, int K, int lb, int ub, Response& response) {
    // 混合键
    std::pair<std::string, std::string> com_key_q = std::make_pair(u_q, type_q);
    // MSA
    MultisetAccumulator msa;
    // 混合键对应的素数
    BigInteger p_bi;
    BigInteger::generate_prime(p_bi, com_key_q.first + com_key_q.second);
    std::string p_str = p_bi.to_string();

    std::vector<std::pair<std::string, int>> res;           // 储存区块链中所有匹配的结果<v,w>
    // 遍历Response中每个区块对应的subtree
    for(int i=lb; i<=ub; i++){
        // 取出该区块对应的subtree
        MHTProof& mht_proof = response.VO[i];

        // 对子树进行验证，并得到所有匹配com_key_q的叶结点
        
        if(traverse(mht_proof.subtree, mht_proof.proof, mht_proof.root_id, res, msa, p_str) != blockchain[i].h_mht){
            std::cout << "Merkle Proof is error" << std::endl;
            return std::vector<std::pair<std::string, int>>();
        }
    }

    // 提取出res中的top-K
    int num_to_be_returned = std::min(K, int(res.size()));
    std::vector<std::pair<std::string, int>> final_result(num_to_be_returned);
    std::partial_sort_copy(res.begin(), res.end(), final_result.begin(), final_result.end(), compare_by_w);

    return final_result;
}




std::string Query::traverse(std::vector<MHTNode>& subtree, std::map<int, Nonmembership_Proof>& proof, 
                                int current_id, std::vector<std::pair<std::string, int>>& res,
                                MultisetAccumulator& msa, std::string p){
    // 判断当前结点是否包含com_key_q
    // 不包含
    if(proof.find(current_id) != proof.end()){
        // 验证non-membership
        if(!msa.verify_nonmembership_prime(subtree[current_id].acc, proof[current_id], p)){
            std::cout<< "Non-Membership verification failed" <<std::endl;
        }

        return subtree[current_id].digest;
    }

    // 包含
    else{
        // 叶结点
        if(subtree[current_id].isLeaf){
            // 将<v,w>加入res
            res.push_back(subtree[current_id].value);
            
            // 计算digest
            std::string msg = std::string(32, '\0') + std::string(32, '\0');
            std::string tmp = Crypto_Primitives::SHA256_digest(msg);
            return Crypto_Primitives::SHA256_digest(tmp + subtree[current_id].acc);
        }
        // 非叶结点
        else{
            std::string lhash = traverse(subtree, proof, subtree[current_id].lchild, res, msa, p);
            std::string rhash = traverse(subtree, proof, subtree[current_id].rchild, res, msa, p);

            std::string tmp = Crypto_Primitives::SHA256_digest(lhash+rhash);
            return Crypto_Primitives::SHA256_digest(tmp + subtree[current_id].acc);
        }
    }
}



bool Query::compare_by_w(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b){
    return a.second > b.second;
}