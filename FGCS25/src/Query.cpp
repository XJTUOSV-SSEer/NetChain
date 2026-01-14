#include "../include/Query.h"
#include <cassert>


std::vector<Block> Query::Search(std::string u_q, std::string type_q, int lb, int ub, std::vector<Block>& chain){
    std::vector<Block> VO;                  // 验证对象，包含多个完整区块
    std::string kw_q = u_q + type_q;        // 查询复合键

    int current_blk_height = ub;        // 当前锚点的块号
    while(current_blk_height >= lb){
        Block& current_blk = chain[current_blk_height];
        VO.push_back(current_blk);          // 当前被访问的区块加入VO

        bool is_match = false;
        // 判断当前区块是否包含kw_q
        if(current_blk.bf.check(kw_q)){            
            // 检查是否确实包含kw_q
            for(transaction& tx : current_blk.transactions){
                if(tx.u == u_q && tx.type == type_q) {
                    is_match = true;
                    break;
                }
            }
        }

        // 若当前块确实包含kw_q，则将锚点前移一个块；否则，检查skip index，计算要将锚点前移多少块
        if(is_match) {
            current_blk_height -= 1;
        }
        else{
            // 遍历skip index
            bool is_find = false;       // 标识是否找到一个BF使得检查为阳性
            for(size_t j = 0; j < current_blk.e_; j++) {
                // 检查第j个BF。若第j个BF的检查通过，则将锚点前移2^j块
                
                if(current_blk.skip_index[j].check(kw_q)) {
                    current_blk_height = current_blk_height - (1<<j);
                    is_find = true;
                    break;
                }
            }
            // 特殊情况：若e个BF检查都为阴性，则将锚点前移2^e块
            if(!is_find) {
                current_blk_height = current_blk_height - (1 << current_blk.e_);
            }
        }
    }

    return VO;
}



std::vector<transaction> Query::Verify(std::string u_q, std::string type_q, int lb, int ub, 
                                    std::vector<Block>& chain, std::vector<Block>& VO){
    // 首先验证VO中的所有区块。对每个区块，验证其中的skip index与所有交易
    for(Block& blk : VO) {
        // 验证skip index
        std::string msg = blk.bf.bin_form();
        for(size_t j=0; j<blk.e_; j++){
            msg += blk.skip_index[j].bin_form();
        }
        assert( Crypto_Primitives::SHA256_digest(msg) == chain[blk.blk_height].h_skip);

        // 验证交易。这里只模拟计算哈希的过程
        msg = "";
        for(transaction& tx : blk.transactions) {
            msg += Crypto_Primitives::SHA256_digest(tx.u + tx.v + tx.type + std::to_string(tx.w));
        }
        std::string txs_digest = Crypto_Primitives::SHA256_digest(msg);
    }

    // rerun查询步骤，确认所有的匹配交易都被返回
    std::string kw_q = u_q + type_q;        // 查询复合键
    std::vector<transaction> results;       // 匹配交易
    
    for(size_t i = 0; i < VO.size(); i++){
        Block& current_blk = VO[i];
        // 判断当前区块是否包含kw_q
        bool is_match = false;
        if(current_blk.bf.check(kw_q)){            
            // 检查是否确实包含kw_q
            for(transaction& tx : current_blk.transactions){
                if(tx.u == u_q && tx.type == type_q) {
                    is_match = true;
                    break;
                }
            }
        }
        
        // 若当前块确实包含kw_q，确认前一个块是否在VO中；否则，检查skip index，计算要将锚点前移x块，然后确认前x个块是否在VO中。
        if(is_match) {
            if(i < VO.size() - 1){
                assert(VO[i+1].blk_height == current_blk.blk_height - 1);
            }
            else{           // 边界条件，VO中最后一个块
                assert(current_blk.blk_height - 1 < lb);
            }   
        }
        else{
            // 遍历skip index
            bool is_find = false;       // 标识是否找到一个BF使得检查为阳性
            int next_blk_height = 0;      // 下一个锚点的块号
            for(size_t j = 0; j < current_blk.e_; j++) {
                // 检查第j个BF。若第j个BF的检查通过，则将锚点前移2^j块                
                if(current_blk.skip_index[j].check(kw_q)) {
                    next_blk_height = current_blk.blk_height - (1<<j);
                    is_find = true;
                    break;
                }
            }
            // 特殊情况：若e个BF检查都为阴性，则将锚点前移2^e块
            if(!is_find) {
                next_blk_height = current_blk.blk_height - (1 << current_blk.e_);
            }

            if(i < VO.size() - 1){
                assert(next_blk_height == VO[i+1].blk_height);
            }
            else{           // 边界条件，VO中最后一个块
                assert(next_blk_height < lb);
            }
        }

        // 若当前区块包含匹配交易，将匹配交易加入results
        for(size_t k=0; k < current_blk.transactions.size(); k++) {
            transaction& tx = current_blk.transactions[k];
            if(tx.u == u_q && tx.type == type_q) {
                results.push_back(tx);
            }
        }
    }

    return results;
}