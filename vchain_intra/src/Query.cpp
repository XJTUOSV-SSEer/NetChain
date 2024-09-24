#include "../include/Query.h"




Response Query::search(std::vector<Block>& blockchain, std::string u_q, std::string type_q, int K, 
                        int lb, int ub){
    // 混合键
    std::pair<std::string, std::string> com_key_q = std::make_pair(u_q, type_q);

    // 遍历所有区块
    for(int i=lb; i<=ub; i++){
        // 取出区块
        Block& blk = blockchain[i];

        // 取出区块中的MHT
        MHT& mht = blk.mht;

        // 对MHT进行搜索，层序遍历
    }
}