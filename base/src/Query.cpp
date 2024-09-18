#include "../include/Query.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "../include/Block.h"
#include "../include/SMT.h"
#include "Structs.h"
#include <iostream>


Response Query::Search(std::string u_q, std::string type_q, int K, int lb, int ub, std::vector<Block>& chain){
    // 要查询的混合键
    std::pair<std::string, std::string> com_key_q = std::make_pair(u_q, type_q);
    // 每个区块的查询结果R
    std::map<int, std::vector<ListNode>> R;
    // 每个区块中com_key_q的存在/不存在证明
    std::map<int, SMTProof> VO;

    // 遍历查询区间[lb, ub]内的所有区块
    for(int i=lb; i<=ub; i++){
        // 当前区块
        Block& blk = chain[i];

        // 提取SMT，判断是否包括混合键
        SMT& smt = blk.smt;
        std::vector<SMTNode>& tree = smt.tree;
        // 二分判断是否包含混合键。具体地，对SMT中的叶结点中的混合键进行二分搜索
        int lid = 0, rid = smt.leaf_num-1;          // lid, rid分别为二分搜索的左右边界
        int target = -1;                             // match的结点在tree数组中的下标
        while(rid >= lid){
            int mid = int((lid+rid)/2);             // mid为当前二分搜索区间的中间点
            if(tree[mid].compound_key == com_key_q){
                target = mid;
                break;
            }
            else if(com_key_q > tree[mid].compound_key){
                lid = mid+1;
            }
            else{
                rid = mid-1;
            }
        }

        // 若混合键不存在，生成SMT的不存在证明。
        if(target == -1){
            SMTProof smtproof = smt.prove_Nonexistence(com_key_q);
            VO[i] = smtproof;
        }


        // 若混合键存在，生成SMT的存在证明，并搜索当前区块的top-K结果
        else{
            SMTProof smtproof = smt.prove_Existence(target);
            VO[i] = smtproof;

            // 当前区块中对应于被查询的混合键的List
            std::vector<ListNode>& list = blk.Lists[com_key_q];

            // 返回min(l ,K) 个ListNode
            std::vector<ListNode> v;            // 储存结果
            int num_to_be_returned = std::min(int(list.size()), K);     // 需要返回的ListNode数量
            for(int j=0; j<num_to_be_returned; j++){
                v.push_back(list[j]);
            }

            R[i] = v;
        }

    }

    // 返回结果
    Response response;
    response.R = R;
    response.VO = VO;
    return response;
}



std::vector<std::pair<std::string, int>> Query::Verify(std::string u_q, std::string type_q, Response& response, 
                                    int K, int lb, int ub, std::vector<Block>& chain){
    // 要查询的混合键
    std::pair<std::string, std::string> com_key_q = std::make_pair(u_q, type_q);

    // 储存所有候选结果<v,w>
    std::vector<std::pair<std::string, int>> Res;

    std::map<int, SMTProof>& VO = response.VO;
    std::map<int, std::vector<ListNode>>& R = response.R;

    /* ----------------------------- 遍历所有区块 ----------------------*/
    for(int i = lb; i<=ub; i++){
        // 判断VO是否存在
        if(VO.find(i) == VO.end()){
            std::cout << "The VO is missed out"<< std::endl;
            return std::vector<std::pair<std::string, int>>();
        }

        // 验证SMT的存在/不存在证明
        // 从块头取出H_SMT
        std::string h_smt = chain[i].h_smt;

        // 当前区块不包含查询的混合键，验证不存在性
        if(R.find(i) == R.end()){
            if(!SMT::verify_Nonexistence(VO[i], h_smt, com_key_q)){
                std::cout << "Non-existence Verification failed" << std::endl;
                return std::vector<std::pair<std::string, int>>();
            }

            // 通过不存在性验证，跳过本轮
            continue;
        }

        // 当前区块包含查询的混合键，验证存在性
        else{
            if(!SMT::verify_Existence(VO[i], h_smt, com_key_q)){
                std::cout << "Existence Verification failed" << std::endl;
                return std::vector<std::pair<std::string, int>>();
            }

            // 通过存在性验证，检查当前区块返回的list
            // 从VO中提取出list的信息：l, h1
            int l = VO[i].subtree[VO[i].leaves[0]].l;
            std::string h1 = VO[i].subtree[VO[i].leaves[0]].h1;

            int num_to_be_returned = std::min(l, K);
            std::vector<ListNode>& list = R[i];
            std::string ptr_pre = h1;
            for(int j=0; j<num_to_be_returned; j++){

                // 对list中每个结点，提取出v,w,ptr
                ListNode& ln = list[j];
                std::string v = ln.v;
                int w = ln.w;
                std::string p = ln.ptr;

                // 计算当前结点的哈希，并与ptr_pre对比
                std::string h = Crypto_Primitives::SHA256_digest(v+std::to_string(w)+p);
                if(ptr_pre != h){
                    std::cout << "hash pointer verification failed" <<std::endl;
                    return std::vector<std::pair<std::string, int>>();
                }

                ptr_pre = p;

                Res.push_back(std::make_pair(v,w));
            }
        }
    }



    /* ---------------------- 从候选结果集Res中选出全局top-K ------------------------------*/
    // 对于特殊情况，即候选结果集中元素数量小于K的，需要设置数量
    int num = std::min((int)Res.size(), K);
    // 最终结果
    std::vector<std::pair<std::string, int>> final_result(num);
    std::partial_sort_copy(Res.begin(), Res.end(), final_result.begin(), final_result.end(), compare_by_w);

    return final_result;
}






bool Query::compare_by_w(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b){
    return a.second > b.second;
}