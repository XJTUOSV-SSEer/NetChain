#include "../include/SkipList.h"
#include <cmath>
#include <algorithm>


SkipList::SkipList(int blk_id, std::vector<Block>& blockchain, int level,
                    std::set<std::pair<std::string, std::string>>& latest_set, 
                    std::map<std::pair<std::string, std::string>, std::string>& prime_map){
    // 对不同的skip，建立SkipListNode
    // skip升序
    for(int i=1; i<=level; i++){
        // 当前的skip = 2^i
        int skip = std::pow(2, i);

        // 判断当前区块及之前的区块数是否小于skip。若小于，则不继续构造该skip的SkipListNode
        if(blk_id+1 < skip){
            SkipListNode node;
            node.is_empty = true;
            node.skip = skip;
        }

        // 构造该skip的SkipListNode
        else{
            SkipListNode node;
            node.is_empty = false;
            node.skip = skip;

            // 将skip内所有区块的混合键求并集
            std::set<std::pair<std::string, std::string>> set_union = latest_set;        // 储存并集
            for(int j = blk_id+1-skip; j<blk_id; j++){
                // 取出该区块中的混合键集合
                MHT& mht = blockchain[j].mht;
                std::set<std::pair<std::string, std::string>>& key_set = mht.tree[mht.root_id].com_key_set;

                // 并集
                std::set<std::pair<std::string, std::string>> tmp_set;
                std::set_union(set_union.begin(), set_union.end(), key_set.begin(), key_set.end(), 
                        std::inserter(tmp_set, tmp_set.begin()));
                set_union = tmp_set;
            }

            // 计算set_union对应的acc和product
            std::set<std::string> x_set;            // 储存集合中混合键对应的素数
            for(std::pair<std::string, std::string> com_key: set_union){
                // 计算混合键对应的素数

                // 计算acc
            }
        }
    }
}