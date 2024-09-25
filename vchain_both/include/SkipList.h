#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <vector>
#include "Block.h"
#include <set>
#include "Structs.h"

// 跳表类
class SkipList{
private:

public:
    // 储存多个跳。5级，skip分别为2, 4, 8, 16, 32
    std::vector<SkipListNode> skiplist;


    /*
        构造函数。为一个区块构造跳表。需要注意，此时blk_id对应的区块还没有加入blockchain
        param:
            blk_id - 当前区块加入后的区块号
            blockchain - 区块链的引用
            level - 跳表的级数
            latest_set - blk_id对应的区块的混合键集合
            prime_map - 储存混合键对应的素数，用于打表
    */
    SkipList(int blk_id, std::vector<Block>& blockchain, int level, 
            std::set<std::pair<std::string, std::string>>& latest_set,
            std::map<std::pair<std::string, std::string>, std::string>& prime_map);

};




#endif