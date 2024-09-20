// class for Merkle Patricia Trie (MPT)

#ifndef MPT_H
#define MPT_H

#include <vector>
#include <string>
#include <set>
#include <map>
#include "Crypto_Primitives.h"
#include "Structs.h"


// 本质上是前缀树+MHT
class MPT{
private:

public:
    /*
        用于前缀树寻找路径。输入路径中的一个字符，返回这个字符在指针数组中对应的下标
    */
    static int get_order(std::string s);

    // 储存MPT树中的结点
    std::vector<MPTNode> tree;

    // 根结点在tree中的下标
    int root_id;
};




#endif