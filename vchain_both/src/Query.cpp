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


    // 使用跳表查询
    // 从后向前搜索跳表
    int current_blk = ub;
    while(current_blk >= lb){
        bool find_jump = false;

        // 当前块的跳表
        SkipList& skiplist = blockchain[current_blk].skiplist;

        // 找到current_blk向前最大的skip
        for(int i = skiplist.vec.size()-1; i>=0; i--){
            // 若当前跳为空，则continue
            if(skiplist.vec[i].is_empty){
                continue;
            }

            // 当前的skip
            int current_skip = skiplist.vec[i].skip;

            // 判断当前skip对应的com_key_set是否包含com_key_q
            // 包含
            if(skiplist.vec[i].com_key_set.find(com_key_q) != skiplist.vec[i].com_key_set.end()){
                continue;
            }
            // 不包含
            else{
                // 生成Non-membership proof
                Nonmembership_Proof nmproof = msa.proove_nonmembership_prime(skiplist.vec[i].product, p_str);

                // 构造VO2
                SkipListProof proof_sl;
                proof_sl.id = i;
                proof_sl.proof = nmproof;
                for(int j=0; j<skiplist.vec.size(); j++){
                    SkipListNode tmpnode;
                    // 对于当前的skip，证明中填充acc
                    if(j==i){
                        tmpnode.skip = current_skip;
                        tmpnode.acc = skiplist.vec[i].acc;                        
                    }
                    // 否则填充digest
                    else{
                        tmpnode.digest = skiplist.vec[j].digest;
                    }
                    proof_sl.vec.push_back(tmpnode);
                }

                response.VO2[current_blk] = proof_sl;

                find_jump = true;
                // 更新current_blk
                current_blk = current_blk - current_skip;
                break;
            }
        }


        // 判断是否找到了skip；若没有，进行块内搜索
        if(!find_jump){
            // 对应于此区块的MHTProof
            MHTProof mht_proof;

            // 取出区块
            Block& blk = blockchain[current_blk];

            // 取出区块中的MHT
            MHT& mht = blk.mht;

            // 对MHT进行搜索，得到结果和证明
            mht_proof.root_id = mht.search(mht_proof, blk.mht.root_id, com_key_q, msa, p_str);

            // 将证明加入VO
            response.VO[current_blk] = mht_proof;

            // 更新current_blk
            current_blk = current_blk - 1;
        }
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
    // 区块链中所有匹配com_key_q的结果
    std::vector<std::pair<std::string, int>> res;


    // 验证
    int current_blk = ub;
    while(current_blk >= lb){
        // 先判断VO2[current_blk]是否为空，若不为空，存在从当前块向前的一跳
        // 存在一跳，验证跳表
        if(response.VO2.find(current_blk) != response.VO2.end()){
            SkipListProof& proof_sl = response.VO2[current_blk];

            // Non-membership验证
            if(!msa.verify_nonmembership_prime(proof_sl.vec[proof_sl.id].acc, proof_sl.proof, p_str)){

                std::cout << "Non-membership verification in skiplist failed" <<std::endl;
                std::cout << proof_sl.id <<std::endl;
                std::cout << proof_sl.vec[proof_sl.id].acc <<std::endl;
                std::cout << current_blk << std::endl;
                return std::vector<std::pair<std::string, int>>();
            }

            // 根据acc填充proof_sl.id中的digest
            proof_sl.vec[proof_sl.id].digest = Crypto_Primitives::SHA256_digest(proof_sl.vec[proof_sl.id].acc);

            // 验证h_sl
            // 根据证明，计算h_sl
            Block& blk = blockchain[current_blk];
            std::string msg = "";
            for(SkipListNode node: proof_sl.vec){
                msg += node.digest;
            }

            if(Crypto_Primitives::SHA256_digest(msg) != blk.h_sl){
                std::cout << "digest of SkipList is incorrect" <<std::endl;
                return std::vector<std::pair<std::string, int>>();
            }

            // 跳表验证通过，更新current_blk
            current_blk = current_blk - proof_sl.vec[proof_sl.id].skip;
        }

        // 不存在一跳，验证块内的MHT
        else{
            // 取出该区块对应的subtree
            MHTProof& mht_proof = response.VO[current_blk];

            // 对子树进行验证，并得到所有匹配com_key_q的叶结点
            if(traverse(mht_proof.subtree, mht_proof.proof, mht_proof.root_id, res, msa, p_str) != 
                        blockchain[current_blk].h_mht){
                std::cout << "Merkle Proof is error" << std::endl;
                return std::vector<std::pair<std::string, int>>();
            }

            // 更新current_blk
            current_blk = current_blk-1;
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