#include "../include/Query.h"
#include <vector>
#include <string>
#include <map>
#include "../include/Block.h"
#include "../include/SMT.h"
#include "Structs.h"


void Query::Search(std::string u_q, std::string type_q, int K, int lb, int ub, std::vector<Block>& chain){
    // 要查询的混合键
    std::pair<std::string, std::string> com_key_q;
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
        }


        // 若混合键存在，生成SMT的存在证明，并搜索当前区块的top-K结果
        else{
            
        }

    }

}