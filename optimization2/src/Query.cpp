#include "../include/Query.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "../include/Block.h"
#include "../include/SMT.h"
#include "Structs.h"
#include <iostream>


Response Query::Search(std::string u_q, std::string type_q, int K, int lb, int ub, std::vector<Block>& chain, 
                        MPT& mpt){
    // 要查询的混合键
    std::pair<std::string, std::string> com_key_q = std::make_pair(u_q, type_q);
    // 每个区块的查询结果R
    std::map<int, std::vector<ListNode>> R;
    // 每个区块中com_key_q的存在证明
    std::map<int, SMTProof> VO;
    // 返回的结果+证明
    Response response;


    // 从MPT中找到com_key_q对应的latest block id
    int id_0 = mpt.search(com_key_q);
    int id = id_0;

    // 若最近的记录所在区块号小于lb，返回只有id_0和VO2的response
    if(id<lb){
        // 生成MPT的存在性证明
        response.VO2 = mpt.proveExistence(com_key_q);
        response.id_0 = id_0;
        return response;
    }

    // 找到恰好在ub范围外且包含com_key_q的区块，并储存在id_0中
    while(id > ub){
        // 取出区块号为id的区块，并从SMT中找出com_key_q对应的叶结点，从中提取出id_pre
        Block& blk = chain[id];
        
        SMT& smt = blk.smt;
        std::vector<SMTNode>& tree = smt.tree;
        // 二分搜索得到键在叶结点中的位置
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

        // 访问目标叶结点
        SMTNode& smtnode = tree[target];
        int id_pre = smtnode.id_pre;

        // 判断id_pre是否在[lb, ub]内
        if(id_pre>=lb && id_pre<=ub){
            id_0 = id;
            response.VO[id] = smt.prove_Existence(target);
        }
        // 特殊情况，id>ub, id_pre<lb。此时仍然对id中的SMT做存在证明，并直接返回
        else if(id_pre<lb){
            response.VO[id] = smt.prove_Existence(target);
            response.id_0 = id;
            return response;
        }

        // 更新id
        id = id_pre;
    }


    // 若id_0在[lb,ub]内，说明混合键对应最新的值在查询区间中，需要根据MPT证明id_0储存了混合键最新的记录
    if(id_0>=lb && id_0<=ub){
        response.VO2 = mpt.proveExistence(com_key_q);
    }





    // 第一遍scan，为包含com_key_q的块生成存在证明
    std::vector<int> global_weights;             // 储存全局所有候选结果中的权重
    std::set<int> candidate_blk_ids;             // 储存所有包含被查询的混合键的block id

    while(id>=lb && id<=ub){
        // 当前区块
        Block& blk = chain[id];
        SMT& smt = blk.smt;
        std::vector<SMTNode>& tree = smt.tree;

        // 二分搜索得到键在叶结点中的位置
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

        // 生成SMT的存在证明，并搜索当前区块的top-K结果
        VO[id] = smt.prove_Existence(target);
        candidate_blk_ids.insert(id);

        // 将list中所有元素的w权重加入global_weights数组
        // 当前区块中对应于被查询的混合键的List
        std::vector<ListNode>& list = blk.Lists[com_key_q];
        for(int j=0; j<list.size(); j++){
            global_weights.push_back(list[j].w);
        }

        // 更新id为id_pre
        id = tree[target].id_pre;
    }



    // 计算全局的top-K的权重w
    // 对于特殊情况，全局所有结果不足K的，则返回全部
    // 计算num_to_be_returned即可
    int num_to_be_returned = std::min(K, int(global_weights.size()));
    std::vector<int> dest(num_to_be_returned);       // 储存全局的top-K元素
    // 将前num_to_be_returned个w复制到dest数组中
    std::partial_sort_copy(global_weights.begin(), global_weights.end(), dest.begin(), dest.end(), std::greater<int>());


    // 第二遍扫描，对包含com_key_q的块扫描，得到每个块应当返回的listnode
    // 为了防止list有多个相同的w，每当list中的元素属于全局top-k时，将全局top-k中相应的元素删除
    for(std::set<int>::iterator it = candidate_blk_ids.begin(); it!= candidate_blk_ids.end(); it++){
        // 当前区块号
        int blk_id = *it;
        // 当前区块的list
        std::vector<ListNode>& list = chain[blk_id].Lists[com_key_q];
        std::vector<ListNode> v;            // 储存结果

        // 判断当前区块为全局的top-K结果贡献多少个元素
        for(int j=0; j<list.size(); j++){
            int w = list[j].w;
            std::vector<int>::iterator iter = std::find(dest.begin(), dest.end(), w);
            // 当前结点在全局结果中
            if(iter != dest.end()){
                // 将该w从全局top-K数组删除，避免后续遇到重复的w
                dest.erase(iter);
            }
            else{
                // 将当前结点以及之前的所有结点加入R[blk_id]
                for(int k=0; k<=j; k++){
                    v.push_back(list[k]);
                }
                break;
            }

            // 特判，当前为list最后一个元素，无论是否在全局top-K都要结束
            if(j==list.size()-1){
                for(int k=0; k<=j; k++){
                    v.push_back(list[k]);
                }
            }
        }

        R[blk_id] = v;
    }



    // 返回结果
    response.R = R;
    response.VO = VO;
    response.id_0 = id_0;
    return response;
}



std::vector<std::pair<std::string, int>> Query::Verify(std::string u_q, std::string type_q, Response& response, 
                                    int K, int lb, int ub, std::vector<Block>& chain){
    // 要查询的混合键
    std::pair<std::string, std::string> com_key_q = std::make_pair(u_q, type_q);

    std::map<int, SMTProof>& VO = response.VO;
    std::map<int, std::vector<ListNode>>& R = response.R;

    // 最终得到的结果
    std::vector<std::pair<std::string, int>> final_result;


    // 首先从R中筛选得到全局top-K结果
    // 由于可能所有的正确结果都不足K，因此计算时需要额外处理
    std::vector<int> top_k_w;               // 储存从response中提取的全局top-K的权重
    for(int i=lb; i<=ub; i++){
        if(R.find(i)==R.end()){
            continue;
        }
        std::vector<ListNode>& list = R[i];
        for(int j=0; j<list.size(); j++){
            top_k_w.push_back(list[j].w);
        }
    }
    // 判断返回的结果是否小于K（特殊情况）
    int num_to_be_returned = std::min(K, int(top_k_w.size()));
    std::vector<int> tmp(num_to_be_returned);
    std::partial_sort_copy(top_k_w.begin(), top_k_w.end(), tmp.begin(), tmp.end(), std::greater<int>());
    top_k_w = tmp;



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

            std::vector<ListNode>& list = R[i];
            std::string ptr_pre = h1;
            for(int j=0; j<list.size(); j++){

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


                // 判断下标j是否符合要求。这里有一个值得注意的问题：若多个list中都包含相同的权重w，但是部分是作为
                // 结果，部分是作为边界，则验证时会造成歧义和错误。因此，之前规定SP按照块的id号顺序生成相应结果与
                // 证明，保证前部分重复的w作为结果，后部分重复的w作为边界，且验证时也按照区块号升序的顺序来验证，
                // 保证正确性

                // j不是list最后一个元素，且在全局top-k中
                if(j<list.size()-1 && std::find(tmp.begin(), tmp.end(), list[j].w)!=tmp.end()){
                    final_result.push_back(std::make_pair(v, w));
                    // 为了应对重复的w，需要将这个元素从top_k_w中删除
                    tmp.erase(std::find(tmp.begin(), tmp.end(), list[j].w));
                }
                // j是最后一个元素，且不在全局top-K中
                else if(j==list.size()-1 && std::find(tmp.begin(), tmp.end(), list[j].w)==tmp.end()) {

                }
                // j是最后一个元素，且在全局top-K中，且该结点哈希指针为空
                else if(j==list.size()-1 && std::find(tmp.begin(), tmp.end(), list[j].w)!=tmp.end() 
                        && list[j].ptr == std::string(32, '\0')){
                    final_result.push_back(std::make_pair(v, w));
                    tmp.erase(std::find(tmp.begin(), tmp.end(), list[j].w));
                }
                // 其他情况都为错误
                else{
                    std::cout << "Verification for R list failed" <<std::endl;
                    return std::vector<std::pair<std::string, int>>();
                }
            }
        }
    }

    return final_result;
}






bool Query::compare_by_w(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b){
    return a.second > b.second;
}